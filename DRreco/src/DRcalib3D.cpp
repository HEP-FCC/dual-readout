#include "DRcalib3D.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TComplex.h"

#include <cmath>

DECLARE_COMPONENT(DRcalib3D)

DRcalib3D::DRcalib3D(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc), m_geoSvc("GeoSvc", aName) {
  declareProperty("GeoSvc", m_geoSvc);

  pSeg = nullptr;
  pParamBase = nullptr;
}

StatusCode DRcalib3D::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  info() << "DRcalib3D initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode DRcalib3D::execute() {
  const edm4hep::RawCalorimeterHitCollection* digiHits = m_digiHits.get();
  const edm4hep::CalorimeterHitCollection* hits2d = m_2dHits.get();
  const edm4hep::SparseVectorCollection* waveforms = m_waveforms.get();
  edm4hep::CalorimeterHitCollection* caloHits = m_caloHits.createAndPut();
  edm4hep::SparseVectorCollection* postprocTimes = m_postprocTime.createAndPut();

  for (unsigned int idx = 0; idx < hits2d->size(); idx++) {
    // WARNING assume same input order (sequential access)
    auto& hit2d = hits2d->at(idx);
    auto& waveform = waveforms->at(idx);

    // sanity check for input order
    auto& digiHit = digiHits->at(idx);

    if ( hit2d.getCellID()!=digiHit.getCellID() ) {
      error() << "Input order between DRcalo2dHits and DigiCalorimeterHits does not agree!" << endmsg;
      return StatusCode::FAILURE;
    }

    // set segmentation parameter
    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( hit2d.getCellID() );
    int numEta = pSeg->numEta(cID);
    int numPhi = pSeg->numPhi(cID);
    pParamBase = pSeg->setParamBase(numEta);

    // estimate fiber geometry
    auto position = hit2d.getPosition();
    auto towerPos = pParamBase->GetTowerPos(numPhi);
    auto waferPos = pParamBase->GetSipmLayerPos(numPhi);
    dd4hep::Position sipmPos(position.x,position.y,position.z); // type cast to dd4hep::Position

    auto fiberDir = waferPos - towerPos; // outward direction
    auto fiberUnit = fiberDir.Unit();

    double speed = pSeg->IsCerenkov(cID) ? m_cherenSpeed.value() : m_scintSpeed.value();
    float effVelocity = speed*dd4hep::millimeter/dd4hep::nanosecond;
    float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;

    // create a histogram to do FFT and fill it
    std::unique_ptr<TH1D> waveHist = std::make_unique<TH1D>("waveHist","waveHist",m_nbins,m_gateStart,m_gateStart+m_gateL);

    for (unsigned int bin = 0; bin < waveform.centers_size(); bin++) {
      float timeBin = waveform.getCenters(bin);
      waveHist->Fill(timeBin,waveform.getContents(bin));
    }

    // Fast Fourier Transform (Z-transform)
    auto* waveProcessed = processFFT(waveHist.get()); // need to delete manually
    double integral = waveProcessed->Integral();

    auto postprocTime = postprocTimes->create();

    for (int bin = 1; bin <= waveProcessed->GetNbinsX(); bin++) {
      double con = waveProcessed->GetBinContent(bin)/static_cast<double>(m_nbins.value());
      double cen = m_gateStart + m_gateL*waveProcessed->GetBinCenter(bin)/static_cast<double>(m_nbins.value());

      if (con < m_thres) continue;

      postprocTime.addToContents( con );
      postprocTime.addToCenters( cen );

      // estimate 3d hit position
      double energy = hit2d.getEnergy()*con/integral;
      double timeBin = cen*dd4hep::nanosecond;
      double numerator = timeBin - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
      dd4hep::Position pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;
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

    delete waveProcessed;
  }

  return StatusCode::SUCCESS;
}

StatusCode DRcalib3D::finalize() { return GaudiAlgorithm::finalize(); }

TH1* DRcalib3D::processFFT(TH1* waveHist) {
  TH1* zMag = 0;
  TVirtualFFT::SetTransform(0);
  zMag = waveHist->FFT(zMag, "MAG M"); // need to delete manually

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

  TVirtualFFT* fft_own = TVirtualFFT::FFT(1, &nbins, "C2R M");
  fft_own->SetPointsComplex(&(shift_re[0]),&(shift_im[0]));
  fft_own->Transform();

  TH1* zAns = 0;
  zAns = TH1::TransformHisto(fft_own, zAns, "MAG M"); // need to delete manually

  return zAns;
}
