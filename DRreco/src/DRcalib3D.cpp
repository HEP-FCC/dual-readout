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
  // edm4hep::CalorimeterHitCollection* caloHits = m_caloHits.createAndPut();
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

    std::unique_ptr<TH1D> waveHist = std::make_unique<TH1D>("waveHist","waveHist",m_nbins,m_gateStart,m_gateStart+m_gateL);

    for (unsigned int bin = 0; bin < waveform.centers_size(); bin++) {
      float timeBin = waveform.getCenters(bin);
      waveHist->Fill(timeBin,waveform.getContents(bin));
    }

    auto* waveProcessed = processFFT(waveHist.get()); // need to delete manually

    auto postprocTime = postprocTimes->create();

    for (int bin = 1; bin <= waveProcessed->GetNbinsX(); bin++) {
      double con = waveProcessed->GetBinContent(bin)/static_cast<double>(m_nbins.value());
      double cen = m_gateStart + m_gateL*waveProcessed->GetBinCenter(bin)/static_cast<double>(m_nbins.value());

      if (con < m_thres) continue;

      postprocTime.addToContents( con );
      postprocTime.addToCenters( cen );
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

// void DRcalib3D::addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input, const float calib) {
//   auto cID = static_cast<dd4hep::DDSegmentation::CellID>(input.getEdm4hepHit().getCellID());
//   int numPhi = pSeg->numPhi( cID );
//   auto towerPos = pParamBase->GetTowerPos(numPhi);
//   auto waferPos = pParamBase->GetSipmLayerPos(numPhi);
//   auto sipmPos_ = pSeg->position(cID);
//   dd4hep::Position sipmPos(sipmPos_.x(),sipmPos_.y(),sipmPos_.z()); // type cast from dd4hep::DDSegmentation::Vector3D to dd4heo::Position
//
//   auto fiberDir = waferPos - towerPos; // outward direction
//   auto fiberUnit = fiberDir.Unit();
//
//   if (pSeg->IsCerenkov(cID)) processCheren(drHit,input,calib,sipmPos,fiberUnit);
//   else processScint(drHit,input,calib,sipmPos,fiberUnit);
// }
//
// void DRcalib3D::processCheren(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
//                                    const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit) {
//   auto& rawhit = input.getEdm4hepHit();
//
//   if (rawhit.getAmplitude() < m_minCheren) {
//     drHit.setValidTimeStruct(0); // too few statistics
//
//     return;
//   }
//
//   float effVelocity = m_cherenSpeed*dd4hep::millimeter/dd4hep::nanosecond;
//   float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;
//
//   for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
//     float edep = static_cast<float>( input.getTimeStruct(bin) )/calib;
//     float timeBin = (input.getTimeBegin(bin)+input.getTimeEnd(bin))*dd4hep::nanosecond/2.; // WARNING caution for unit!
//
//     float numerator = timeBin - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
//     auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;
//
//     edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
//                              pos.y() * CLHEP::millimeter/dd4hep::millimeter,
//                              pos.z() * CLHEP::millimeter/dd4hep::millimeter);
//
//     drHit.addToTimeStruct( edep );
//     drHit.addToPosition( posEdm );
//   }
//
//   drHit.setValidTimeStruct(2); // ok
// }
//
// void DRcalib3D::processScint(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
//                                   const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit) {
//   auto& rawhit = input.getEdm4hepHit();
//
//   if (rawhit.getAmplitude() < m_minScint) {
//     drHit.setValidTimeStruct(0); // too few statistics
//
//     return;
//   }
//
//   float effVelocity = m_scintSpeed*dd4hep::millimeter/dd4hep::nanosecond;
//   float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;
//
//   float totEdep = static_cast<float>(rawhit.getAmplitude())/calib;
//
//   double rangeUpper_wide = m_wideFactor*(m_fitRangeUpper-m_fitRangeLower)+m_fitRangeLower;
//   std::unique_ptr<TH1D> timeHist = std::make_unique<TH1D>("timeHist","timeHist",m_nbins,m_fitRangeLower,m_fitRangeUpper);
//   std::unique_ptr<TH1D> timeHist_wide = std::make_unique<TH1D>("timeHist_wide","timeHist_wide",m_nbins,m_fitRangeLower,rangeUpper_wide);
//
//   for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
//     float content = input.getTimeStruct(bin);
//     float timeBin = (input.getTimeBegin(bin)+input.getTimeEnd(bin))/2.; // in [ns]
//
//     int idxBin_narrow = timeHist->GetXaxis()->FindFixBin(timeBin);
//     int idxBin_wide = timeHist_wide->GetXaxis()->FindFixBin(timeBin);
//
//     timeHist->AddBinContent(idxBin_narrow,content);
//     timeHist_wide->AddBinContent(idxBin_wide,content);
//   }
//
//   std::vector<double> srcMarkov(m_nbins,0.);
//   for (int bin = 1; bin <= timeHist->GetNbinsX(); bin++)
//     srcMarkov[bin-1] = timeHist->GetBinContent(bin);
//
//   for (int bin = 1; bin <= timeHist->GetNbinsX(); bin++)
//     timeHist->SetBinContent(bin,srcMarkov[bin-1]);
//
//   //
//
//   double targetsum = processThreshold(targetMax,target_preprocess,target_postprocess); // cut threshold once more
//
//   if ( targetsum <= 0. ) {
//     drHit.setValidTimeStruct(0); // too few statistics
//
//     return;
//   }
//
//   for (auto ts = target_postprocess.begin(); ts != target_postprocess.end(); ++ts) {
//     float numerator = ts->first - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
//     auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;
//
//     edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
//                              pos.y() * CLHEP::millimeter/dd4hep::millimeter,
//                              pos.z() * CLHEP::millimeter/dd4hep::millimeter);
//
//     drHit.addToTimeStruct( (ts->second)*totEdep/targetsum );
//     drHit.addToPosition( posEdm );
//   }
//
//   drHit.setValidTimeStruct(2);
// }
//
// bool DRcalib3D::pruneTail(const dd4hep::Position& pos, const dd4hep::Position& fiberUnit) {
//   double innerProduct = pos.Dot(fiberUnit);
//   return innerProduct > pParamBase->GetCurrentInnerR() - m_pruneTolerance*dd4hep::millimeter;
// }
//
// edm4hep::Vector3f DRcalib3D::getPosition(dd4hep::DDSegmentation::CellID& cID) {
//   auto globalPos = pSeg->position( cID );
//   return { static_cast<float>( globalPos.x() * CLHEP::millimeter/dd4hep::millimeter ),
//            static_cast<float>( globalPos.y() * CLHEP::millimeter/dd4hep::millimeter ),
//            static_cast<float>( globalPos.z() * CLHEP::millimeter/dd4hep::millimeter ) };
// }
