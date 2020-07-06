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
    // if (towerNo > 0) continue;

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

    if (towerNo != 0 && towerNo != 47) continue;

    implementFibers(towerVol,tower);

    for (int nPhi = 0; nPhi < 1/*fX_towerDim->nphi()*/; nPhi++) {
      dd4hep::PlacedVolume towerPhys = fExperimentalHall->placeVolume( towerVol, towerNoLR*fX_towerDim->nphi()+nPhi, fParamBarrel->GetTransform3D(nPhi) );
      towerPhys.addPhysVolID("tower",towerNoLR*fX_towerDim->nphi()+nPhi);
    }
  }
}

void ddDRcalo::DRconstructor::implementFibers(dd4hep::Volume& towerVol, dd4hep::Trap& trap) {
  xml_comp_t x_cladC ( fX_towerDim->child( _Unicode(cladC) ) );
  xml_comp_t x_coreC ( fX_towerDim->child( _Unicode(coreC) ) );
  xml_comp_t x_coreS ( fX_towerDim->child( _Unicode(coreS) ) );

  dd4hep::Tube fiber = dd4hep::Tube(0.,x_cladC.rmax(),fX_towerDim->height()/2.);
  dd4hep::Tube fiberC = dd4hep::Tube(0.,x_coreC.rmin(),fX_towerDim->height()/2.);
  dd4hep::Tube fiberS = dd4hep::Tube(0.,x_coreS.rmin(),fX_towerDim->height()/2.);

  float sipmSize = fX_towerDim->dx();
  float gridSize = fX_towerDim->distance();

  int numx = static_cast<int>( std::floor( ( fParamBarrel->GetH2()*2. - sipmSize )/gridSize ) ) + 1; // in eta direction
  int numy = static_cast<int>( std::floor( ( fParamBarrel->GetTl2()*2. - sipmSize )/gridSize ) ) + 1; // in phi direction

  std::vector<float> gridX, gridY;

  for (int j = 0; j < numy; j++) {
    for (int k = 0; k < numx; k++) {
      float ptX = -gridSize*static_cast<float>(numx/2) + static_cast<float>(k)*gridSize + ( numx%2==0 ? gridSize/2. : 0. );
      float ptY = -gridSize*static_cast<float>(numy/2) + static_cast<float>(j)*gridSize + ( numy%2==0 ? gridSize/2. : 0. );
      gridX.push_back(ptX);
      gridY.push_back(ptY);
    }
  }

  for (unsigned int j = 0; j < gridX.size(); j++) {
    auto colrow = GetColRowFromCopyNo(static_cast<int>(j), numx);
    int column = colrow.first;
    int row = colrow.second;

    dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
    dd4hep::Position pos = dd4hep::Position(gridX.at(j),gridY.at(j),0.);
    dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

    if ( IsCerenkov(column,row) ) { //c fibre
      dd4hep::IntersectionSolid intersectClad("cladC",fiber,trap,trans);
      dd4hep::Volume cladVol("cladC", intersectClad, fDescription->material(x_cladC.materialStr()));
      cladVol.setVisAttributes(*fDescription, x_cladC.visStr());
      towerVol.placeVolume( cladVol, j, trans );

      dd4hep::IntersectionSolid intersectCore("coreC",fiberC,trap,trans);
      dd4hep::Volume coreVol("coreC", intersectCore, fDescription->material(x_coreC.materialStr()));
      coreVol.setVisAttributes(*fDescription, x_coreC.visStr());
      cladVol.placeVolume( coreVol, j );

      // fCerenRegion->AddRootLogicalVolume(cladLogical);
      // fCerenRegion->AddRootLogicalVolume(coreLogical);
      // cladLogical->SetRegion(fCerenRegion);
      // coreLogical->SetRegion(fCerenRegion);

    } else { // s fibre
      dd4hep::IntersectionSolid intersectClad("cladS",fiber,trap,trans);
      dd4hep::Volume cladVol("cladS", intersectClad, fDescription->material(x_coreC.materialStr()));
      cladVol.setVisAttributes(*fDescription, x_coreC.visStr());
      towerVol.placeVolume( cladVol, j, trans );

      dd4hep::IntersectionSolid intersectCore("coreS",fiberS,trap,trans);
      dd4hep::Volume coreVol("coreS", intersectCore, fDescription->material(x_coreS.materialStr()));
      coreVol.setVisAttributes(*fDescription, x_coreS.visStr());
      cladVol.placeVolume( coreVol, j );

      // fScintRegion->AddRootLogicalVolume(cladLogical);
      // fScintRegion->AddRootLogicalVolume(coreLogical);
      // cladLogical->SetRegion(fScintRegion);
      // coreLogical->SetRegion(fScintRegion);

    }
  }
}

std::pair<int,int> ddDRcalo::DRconstructor::GetColRowFromCopyNo(int copyNo, int numx) {
  int column = copyNo % numx;
  int row = copyNo / numx;

  return std::make_pair(column,row);
}

bool ddDRcalo::DRconstructor::IsCerenkov(int col, int row) {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}
