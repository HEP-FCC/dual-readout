#include "DRconstructor.h"

ddDRcalo::DRconstructor::DRconstructor() {
  fX_towerDim = nullptr;
  fX_sipmDim = nullptr;
  fExperimentalHall = nullptr;
  fParamBarrel = nullptr;
  fDescription = nullptr;
  fDetElement = nullptr;
  fSipmSurf = nullptr;
  fFilterSurf = nullptr;
  fNumx = 0;
  fNumy = 0;
  fTowerNoLR = -999;
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
    fTowerNoLR = fParamBarrel->GetIsRHS() ? towerNo : -towerNo-1;

    dd4hep::Trap tower( fX_towerDim->height()/2., 0., 0., fParamBarrel->GetH1(), fParamBarrel->GetBl1(), fParamBarrel->GetTl1(), 0.,
                        fParamBarrel->GetH2(), fParamBarrel->GetBl2(), fParamBarrel->GetTl2(), 0. );

    dd4hep::Volume towerVol( "tower"+std::to_string(fTowerNoLR), tower, fDescription->material(fX_towerDim->materialStr()) );
    towerVol.setVisAttributes(*fDescription, fX_towerDim->visStr());

    if (towerNo != 0 && towerNo != 47) continue;

    implementFibers(towerVol,tower);

    dd4hep::Box sipmLayer( fParamBarrel->GetH2(), fParamBarrel->GetBl2(), fParamBarrel->GetSipmHeight()/2. );
    dd4hep::Volume sipmLayerVol( "sipmLayer"+std::to_string(fTowerNoLR), sipmLayer, fDescription->material(fX_sipmDim->materialStr()) );
    sipmLayerVol.setVisAttributes(*fDescription, fX_sipmDim->visStr());

    implementSipms(sipmLayerVol);

    for (int nPhi = 0; nPhi < 1/*fX_towerDim->nphi()*/; nPhi++) {
      dd4hep::PlacedVolume towerPhys = fExperimentalHall->placeVolume( towerVol, fTowerNoLR*fX_towerDim->nphi()+nPhi, fParamBarrel->GetTransform3D(nPhi) );
      towerPhys.addPhysVolID("tower",fTowerNoLR*fX_towerDim->nphi()+nPhi);

      dd4hep::PlacedVolume sipmLayerPhys = fExperimentalHall->placeVolume( sipmLayerVol, fTowerNoLR*fX_towerDim->nphi()+nPhi, fParamBarrel->GetSipmTransform3D(nPhi) );
      sipmLayerPhys.addPhysVolID("sipmLayer",fTowerNoLR*fX_towerDim->nphi()+nPhi);
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

  fNumx = static_cast<int>( std::floor( ( fParamBarrel->GetH2()*2. - sipmSize )/gridSize ) ) + 1; // in eta direction
  fNumy = static_cast<int>( std::floor( ( fParamBarrel->GetBl2()*2. - sipmSize )/gridSize ) ) + 1; // in phi direction

  fGridX.clear();
  fGridY.clear();

  for (int j = 0; j < fNumy; j++) {
    for (int k = 0; k < fNumx; k++) {
      float ptX = -gridSize*static_cast<float>(fNumx/2) + static_cast<float>(k)*gridSize + ( fNumx%2==0 ? gridSize/2. : 0. );
      float ptY = -gridSize*static_cast<float>(fNumy/2) + static_cast<float>(j)*gridSize + ( fNumy%2==0 ? gridSize/2. : 0. );
      fGridX.push_back(ptX);
      fGridY.push_back(ptY);
    }
  }

  for (unsigned int j = 0; j < fGridX.size(); j++) {
    auto colrow = GetColRowFromCopyNo(static_cast<int>(j), fNumx);
    int column = colrow.first;
    int row = colrow.second;

    dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
    dd4hep::Position pos = dd4hep::Position(fGridX.at(j),fGridY.at(j),0.);
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

void ddDRcalo::DRconstructor::implementSipms(dd4hep::Volume& sipmLayerVol) {
  xml_comp_t x_glass ( fX_sipmDim->child( _Unicode(sipmGlass) ) );
  xml_comp_t x_wafer ( fX_sipmDim->child( _Unicode(sipmWafer) ) );
  xml_comp_t x_filter ( fX_sipmDim->child( _Unicode(filter) ) );

  float sipmSize = fX_towerDim->dx();

  for (unsigned int j = 0; j < fGridX.size(); j++) {
    auto colrow = GetColRowFromCopyNo(static_cast<int>(j), fNumx);
    int column = colrow.first;
    int row = colrow.second;

    dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
    dd4hep::Position pos = dd4hep::Position(fGridX.at(j),fGridY.at(j),0.);
    dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

    dd4hep::Box sipmEnvelop(sipmSize/2., sipmSize/2., fParamBarrel->GetSipmHeight()/2.);
    dd4hep::Volume sipmEnvelopVol( "sipmEnvelop", sipmEnvelop, fDescription->material(x_glass.materialStr()) );
    sipmEnvelopVol.setVisAttributes(*fDescription, fX_sipmDim->visStr());
    sipmLayerVol.placeVolume( sipmEnvelopVol, trans );

    dd4hep::Box sipmWaferBox( sipmSize/2., sipmSize/2., x_wafer.height()/2. );
    dd4hep::Volume sipmWaferVol( "sipmWafer", sipmWaferBox, fDescription->material(x_wafer.materialStr()) );
    sipmWaferVol.setVisAttributes(*fDescription, x_wafer.visStr());
    sipmEnvelopVol.placeVolume( sipmWaferVol, j, dd4hep::Position(0., 0., (fParamBarrel->GetSipmHeight()-x_wafer.height())/2.) );
    dd4hep::SkinSurface(*fDescription, *fDetElement, "SiPMSurf_Tower"+std::to_string(fTowerNoLR)+"SiPM"+std::to_string(j), *fSipmSurf, sipmWaferVol);

    if ( IsCerenkov(column,row) ) { //c channel
      dd4hep::Box sipmGlassBox( sipmSize/2., sipmSize/2., (x_glass.height()+x_filter.height())/2. );
      dd4hep::Volume sipmGlassVol( "sipmGlass", sipmGlassBox, fDescription->material(x_glass.materialStr()) );
      sipmGlassVol.setVisAttributes(*fDescription, x_glass.visStr());
      sipmEnvelopVol.placeVolume( sipmGlassVol, dd4hep::Position(0., 0., -x_wafer.height()/2.) );
    } else { // s channel
      dd4hep::Box sipmGlassBox( sipmSize/2., sipmSize/2., x_glass.height()/2. );
      dd4hep::Volume sipmGlassVol( "sipmGlass", sipmGlassBox, fDescription->material(x_glass.materialStr()) );
      sipmGlassVol.setVisAttributes(*fDescription, x_glass.visStr());
      dd4hep::PlacedVolume sipmGlassPlaced = sipmEnvelopVol.placeVolume( sipmGlassVol, dd4hep::Position(0., 0., (x_filter.height()-x_wafer.height())/2.) );

      dd4hep::Box filterBox( sipmSize/2., sipmSize/2., x_filter.height()/2. );
      dd4hep::Volume filterVol( "filter", filterBox, fDescription->material(x_filter.materialStr()) );
      filterVol.setVisAttributes(*fDescription, x_filter.visStr());
      dd4hep::PlacedVolume filterPlaced = sipmEnvelopVol.placeVolume( filterVol, dd4hep::Position(0., 0., -(fParamBarrel->GetSipmHeight()-x_filter.height())/2.) );
      dd4hep::BorderSurface(*fDescription, *fDetElement, "FilterSurf_Tower"+std::to_string(fTowerNoLR)+"SiPM"+std::to_string(j), *fFilterSurf, filterPlaced, sipmGlassPlaced);
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
