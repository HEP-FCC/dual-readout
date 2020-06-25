#include "DRconstructor.h"

ddDRcalo::DRconstructor::DRconstructor() {
  fX_towerDim = nullptr;
  fExperimentalHall = nullptr;
  fParamBarrel = nullptr;
  fDescription = nullptr;
}

void ddDRcalo::DRconstructor::construct() {
  double currentTheta = 0.;
  int towerNo = 0;
  for (xml_coll_t x_dThetaColl(*fX_towerDim,_U(deltatheta)); x_dThetaColl; ++x_dThetaColl, ++towerNo ) {
    // if (towerNo > 30) continue;

    xml_comp_t x_deltaTheta = x_dThetaColl;

    fParamBarrel->SetDeltaTheta(x_deltaTheta.deltatheta());

    double currentToC = currentTheta + x_deltaTheta.deltatheta()/2.;
    currentTheta += x_deltaTheta.deltatheta();
    fParamBarrel->SetThetaOfCenter(currentToC);
    fParamBarrel->init();
    int towerNoLR = fParamBarrel->GetIsRHS() ? towerNo : -towerNo-1;

    dd4hep::Trap tower( fX_towerDim->height()/2., 0., 0., fParamBarrel->GetH1(), fParamBarrel->GetBl1(), fParamBarrel->GetTl1(), 0.,
                        fParamBarrel->GetH2(), fParamBarrel->GetBl2(), fParamBarrel->GetTl2(), 0. );

    dd4hep::Volume towerVol( "tower"+std::to_string(towerNoLR), tower, fDescription->material(fX_towerDim->materialStr()) );
    towerVol.setVisAttributes(*fDescription, fX_towerDim->visStr());

    for (int nPhi = 0; nPhi < 30/*fX_towerDim->nphi()*/; nPhi++) {
      dd4hep::PlacedVolume towerPhys = fExperimentalHall->placeVolume( towerVol, towerNoLR*fX_towerDim->nphi()+nPhi, fParamBarrel->GetTransform3D(nPhi) );
      towerPhys.addPhysVolID("tower",towerNoLR*fX_towerDim->nphi()+nPhi);
    }
  }
}
