#include "DRcalib3D.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TComplex.h"
#include "TFile.h"

#include <cmath>
#include <limits>

DECLARE_COMPONENT(DRcalib3D)

DRcalib3D::DRcalib3D(const std::string& aName, ISvcLocator* aSvcLoc) : Gaudi::Algorithm(aName, aSvcLoc), m_geoSvc("GeoSvc", aName) {
  declareProperty("GeoSvc", m_geoSvc);

  pSeg = nullptr;
  pParamBase = nullptr;
}

StatusCode DRcalib3D::initialize() {
  StatusCode sc = Gaudi::Algorithm::initialize();

  if (sc.isFailure()) return sc;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->getDetector()->readout(m_readoutName).segmentation().segmentation());

  auto veloFile = std::make_unique<TFile>(m_veloFile.value().c_str(),"READ");
  m_veloC.reset( static_cast<TH1D*>(veloFile->Get(m_cherenProf.value().c_str())) );
  m_veloS.reset( static_cast<TH1D*>(veloFile->Get(m_scintProf.value().c_str())) );
  m_veloC->SetDirectory(0);
  m_veloS->SetDirectory(0);
  veloFile->Close();

  info() << "DRcalib3D initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode DRcalib3D::execute(const EventContext&) const {
  const edm4hep::CalorimeterHitCollection* digiHits = m_digiHits.get();
  const edm4hep::CalorimeterHitCollection* hits2d = m_2dHits.get();
  const edm4hep::TimeSeriesCollection* waveforms = m_waveforms.get();
  edm4hep::CalorimeterHitCollection* caloHits = m_caloHits.createAndPut();
  edm4hep::TimeSeriesCollection* postprocTimes = m_postprocTime.createAndPut();

  for (unsigned int idx = 0; idx < hits2d->size(); idx++) {
    // WARNING assume same input order (sequential access)
    const auto& hit2d = hits2d->at(idx);
    const auto& waveform = waveforms->at(idx);

    // sanity check for input order
    const auto& digiHit = digiHits->at(idx);

    if ( hit2d.getCellID()!=digiHit.getCellID() ) {
      error() << "Input order between DRcalo2dHits and DigiCalorimeterHits does not agree!" << endmsg;
      return StatusCode::FAILURE;
    }

    if ( static_cast<float>(m_gateStart)!=waveform.getTime() ) {
      error() << "Gate start is not equal to the starting time of the waveform! waveform.getTime() = " << waveform.getTime() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( static_cast<float>(m_sampling)!=waveform.getInterval() ) {
      error() << "Sampling rate is different! waveform.getInterval() = " << waveform.getInterval() << endmsg;
      return StatusCode::FAILURE;
    }

    if ( static_cast<unsigned>(m_nbins)!=waveform.amplitude_size() ) {
      error() << "Gate length is different! waveform.amplitude_size() = " << waveform.amplitude_size() << endmsg;
      return StatusCode::FAILURE;
    }

    auto postprocTime = postprocTimes->create(); // create an object even if integral is 0 to match the order with other collections
    postprocTime.setInterval( waveform.getInterval() );
    postprocTime.setCellID( waveform.getCellID() );
    postprocTime.setTime( waveform.getTime() );
    double amplitude = static_cast<double>(digiHit.getEnergy());

    if (amplitude <= 0.)
      continue;

    // set segmentation parameter
    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( hit2d.getCellID() );
    int numEta = pSeg->numEta(cID);
    int numPhi = pSeg->numPhi(cID);
    pParamBase = pSeg->setParamBase(numEta);

    // estimate fiber geometry
    auto position = hit2d.getPosition();
    auto towerPos = pParamBase->GetTowerPos(numPhi);
    auto waferPos = pParamBase->GetSipmLayerPos(numPhi);
    dd4hep::Position sipmPos(position.x * dd4hep::millimeter/CLHEP::millimeter,
                             position.y * dd4hep::millimeter/CLHEP::millimeter,
                             position.z * dd4hep::millimeter/CLHEP::millimeter); // type cast to dd4hep::Position

    auto fiberDir = waferPos - towerPos; // outward direction
    auto fiberUnit = fiberDir.Unit();

    double towerH = pParamBase->GetTowerH();
    double scale = pSeg->IsCerenkov(cID) ? m_cherenScale.value() : m_scintScale.value();

    // create a histogram to do FFT and fill it
    std::unique_ptr<TH1D> waveHist = std::make_unique<TH1D>("waveHist","waveHist",m_nbins,m_gateStart,m_gateStart.value()+m_gateL.value());
    float sampling = waveform.getInterval();
    float startTime = waveform.getTime();

    for (unsigned int bin = 0; bin < waveform.amplitude_size(); bin++) {
      float timeBin = startTime + (static_cast<float>(bin)+0.5)*sampling;

      if ( waveform.getAmplitude(bin) > 0. )
        waveHist->Fill(timeBin,waveform.getAmplitude(bin));
    }

    // Fast Fourier Transform (Z-transform)
    std::unique_ptr<TH1> waveProcessed( processFFT(waveHist.get()) );
    double integral = waveProcessed->Integral();
    double toaProc = m_gateStart + m_gateL*waveProcessed->GetBinCenter(waveProcessed->FindFirstBinAbove(0.))/static_cast<double>(m_nbins.value());

    if ( integral > 0. ) {
      for (int bin = 1; bin <= waveProcessed->GetNbinsX(); bin++) {
        double con = waveProcessed->GetBinContent(bin)*amplitude/integral;
        double cen = m_gateStart + m_gateL*waveProcessed->GetBinCenter(bin)/static_cast<double>(m_nbins.value());

        postprocTime.addToAmplitude( con );

        if (con==0.)
          continue;

        // scale effective velocity
        double veloScaled = pSeg->IsCerenkov(cID) ? m_veloC->Interpolate(cen-toaProc) : m_veloS->Interpolate(cen-toaProc);
        veloScaled *= dd4hep::millimeter/dd4hep::nanosecond;
        double invVminusInvC = (veloScaled > 0.) ? 1./veloScaled - 1./dd4hep::c_light : std::numeric_limits<double>::max();

        // estimate 3d hit position
        double energy = hit2d.getEnergy()*con/amplitude;
        double timeBin = cen*dd4hep::nanosecond;
        double numerator = timeBin - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
        dd4hep::Position pos = sipmPos + ( (scale-1.)*towerH - scale*( numerator/invVminusInvC ) )*fiberUnit;
        edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                                 pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                                 pos.z() * CLHEP::millimeter/dd4hep::millimeter);

        auto caloHit = caloHits->create();
        caloHit.setPosition( posEdm );
        caloHit.setCellID( hit2d.getCellID() );
        caloHit.setTime( hit2d.getTime() );
        caloHit.setType( hit2d.getType() );
        caloHit.setEnergy( energy );
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DRcalib3D::finalize() { return Gaudi::Algorithm::finalize(); }

TH1* DRcalib3D::processFFT(TH1* waveHist) const {
  int firstBin = waveHist->FindFirstBinAbove( m_zero.value()*waveHist->GetMaximum() );
  int lastBin = firstBin;

  // define the signal range
  for (int bin = waveHist->GetNbinsX(); bin > firstBin; bin--) {
    double con = waveHist->GetBinContent(bin);

    if ( con > m_zero.value()*waveHist->GetMaximum() ) {
      lastBin = bin;
      break;
    }
  }

  TH1* zMag = 0;
  TVirtualFFT::SetTransform(0);
  zMag = waveHist->FFT(zMag, "MAG M"); // need to delete manually

  // shift the histogram range from [0, 2*freq_xmax] to [-freq_xmax, freq_xmax]
  double freq_xmax = zMag->GetXaxis()->GetXmax()/2.;
  std::unique_ptr<TH1D> mirror = std::make_unique<TH1D>("mirror","mirror",zMag->GetNbinsX(),-freq_xmax,freq_xmax);
  for (int bin = 1; bin <= zMag->GetNbinsX(); bin++) {
    double cen = zMag->GetBinCenter(bin);
    double con = zMag->GetBinContent(bin);

    double cen_new = cen;

    if( cen > freq_xmax )
      cen_new = cen-2.*freq_xmax;

    int bin_new = mirror->GetXaxis()->FindFixBin(cen_new);
    mirror->SetBinContent(bin_new,con);
  }

  // estimate FWHM and exponential decay term
  int nbins = m_nbins.value();
  double mirrorMax = mirror->GetMaximum()/1.41422;
  int fwhmBegin = mirror->FindFirstBinAbove(mirrorMax);
  int fwhmEnd = mirror->FindLastBinAbove(mirrorMax);
  double fwhm_i = (mirror->GetBinCenter(fwhmBegin)+mirror->GetBinCenter(fwhmBegin-1))/2.;
  double fwhm_f = (mirror->GetBinCenter(fwhmEnd)+mirror->GetBinCenter(fwhmEnd+1))/2.;
  double fwhm = fwhm_f-fwhm_i;
  double decay = std::exp( -std::acosh(2.-std::cos(M_PI*fwhm/static_cast<double>(nbins))) );

  TVirtualFFT* fft = TVirtualFFT::GetCurrentTransform();
  std::vector<double> re_full(nbins,0.);
  std::vector<double> im_full(nbins,0.);
  fft->GetPointsComplex(&(re_full[0]),&(im_full[0]));

  int nbins_new = nbins/2+1;
  std::vector<double> shift_re;
  std::vector<double> shift_im;
  shift_re.reserve(nbins_new);
  shift_im.reserve(nbins_new);

  // cancel exponential decay term
  for (int idx = 0; idx < nbins_new; idx++) {
    auto in = TComplex(re_full[idx],im_full[idx]);
    double freq = 2.*M_PI*zMag->GetBinCenter(idx+1)/static_cast<double>(nbins);

    TComplex expterm  = decay*TComplex(std::cos(freq),-std::sin(freq));
    TComplex denominator = 1. - expterm;
    TComplex shift = in*denominator;

    shift_re.emplace_back( shift.Re() );
    shift_im.emplace_back( shift.Im() );
  }

  delete zMag;

  // reverse FFT
  TVirtualFFT* fft_own = TVirtualFFT::FFT(1, &nbins, "C2R M");
  fft_own->SetPointsComplex(&(shift_re[0]),&(shift_im[0]));
  fft_own->Transform();

  TH1* zAns = 0;
  zAns = TH1::TransformHisto(fft_own, zAns, "MAG M"); // need to delete manually

  // remove contents outside the signal range
  for (int bin = 1; bin <= zAns->GetNbinsX(); bin++) {
    if (bin < firstBin)
      zAns->SetBinContent(bin,0.);

    if (bin > lastBin)
      zAns->SetBinContent(bin,0.);
  }

  int maxBin = zAns->GetMaximumBin();
  double peak = zAns->GetMaximum();
  double thres = peak*m_zero.value();
  bool zero = false;

  // remove contents after the first zero
  for (int bin = maxBin; bin <= zAns->GetNbinsX(); bin++) {
    double con = zAns->GetBinContent(bin);

    // find the first zero (under threshold)
    if (con < thres)
      zero = true;

    if (zero)
      zAns->SetBinContent(bin,0.);
  }

  return zAns;
}
