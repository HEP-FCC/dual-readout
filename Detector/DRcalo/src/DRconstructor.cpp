#include "DRconstructor.h"

ddDRcalo::DRconstructor::DRconstructor(xml_det_t& x_det)
: fX_det(x_det),
  // no default initializer for xml_comp_t
  fX_barrel( x_det.child( _Unicode(barrel) ) ),
  fX_endcap( x_det.child( _Unicode(endcap) ) ),
  fX_sipmDim( x_det.child( _Unicode(sipmDim) ) ),
  fX_struct( x_det.child( _Unicode(structure) ) ),
  fX_dim( fX_struct.child( _Unicode(dim) ) ),
  fX_cladC( fX_struct.child( _Unicode(cladC) ) ),
  fX_coreC( fX_struct.child( _Unicode(coreC) ) ),
  fX_coreS( fX_struct.child( _Unicode(coreS) ) ) {
  fExperimentalHall = nullptr;
  fParamBarrel = nullptr;
  fDescription = nullptr;
  fDetElement = nullptr;
  fSensDet = nullptr;
  fSipmSurf = nullptr;
  fFilterSurf = nullptr;
  fSegmentation = nullptr;
  fVis = false;
  fNumx = 0;
  fNumy = 0;
  fTowerNoLR = -999;
}

void ddDRcalo::DRconstructor::construct() {
  // set vis on/off
  fVis = fDescription->visAttributes(fX_det.visStr()).showDaughters();

  implementTowers(fX_barrel, fParamBarrel);
  implementTowers(fX_endcap, fParamEndcap);
}

void ddDRcalo::DRconstructor::implementTowers(xml_comp_t& x_theta, dd4hep::DDSegmentation::DRparamBase* param) {
  double currentTheta = x_theta.theta();
  int towerNo = x_theta.start();
  for (xml_coll_t x_dThetaColl(x_theta,_U(deltatheta)); x_dThetaColl; ++x_dThetaColl, ++towerNo ) {
    xml_comp_t x_deltaTheta = x_dThetaColl;

    param->SetDeltaTheta(x_deltaTheta.deltatheta());

    double currentToC = currentTheta + x_deltaTheta.deltatheta()/2.;
    currentTheta += x_deltaTheta.deltatheta();
    param->SetThetaOfCenter(currentToC);
    param->init();
    fTowerNoLR = param->signedTowerNo(towerNo);

    dd4hep::Trap assemblyEnvelop( (x_theta.height()+param->GetSipmHeight())/2., 0., 0., param->GetH1(), param->GetBl1(), param->GetTl1(), 0.,
                                  param->GetH2sipm(), param->GetBl2sipm(), param->GetTl2sipm(), 0. );

    dd4hep::Trap tower( x_theta.height()/2., 0., 0., param->GetH1(), param->GetBl1(), param->GetTl1(), 0.,
                        param->GetH2(), param->GetBl2(), param->GetTl2(), 0. );

    dd4hep::Volume towerVol( "tower", tower, fDescription->material(x_theta.materialStr()) );
    towerVol.setVisAttributes(*fDescription, x_theta.visStr());

    implementFibers(x_theta, towerVol, tower, param);

    xml_comp_t x_wafer ( fX_sipmDim.child( _Unicode(sipmWafer) ) );

    // Assume the top surface is nearly rectangular shape
    dd4hep::Box sipmLayer( param->GetBl2(), param->GetH2(), (param->GetSipmHeight()-x_wafer.height())/2. );
    dd4hep::Volume sipmLayerVol( "sipmLayer", sipmLayer, fDescription->material(fX_sipmDim.materialStr()) );
    if (fVis) sipmLayerVol.setVisAttributes(*fDescription, fX_sipmDim.visStr());

    // Photosensitive wafer
    float gridSize = fX_dim.distance();
    double waferX = static_cast<double>(fNumx)*gridSize;
    double waferY = static_cast<double>(fNumy)*gridSize;
    dd4hep::Box sipmWaferBox( waferX/2., waferY/2., x_wafer.height()/2. );
    dd4hep::Volume sipmWaferVol( "sipmWafer", sipmWaferBox, fDescription->material(x_wafer.materialStr()) );
    if (fVis) sipmWaferVol.setVisAttributes(*fDescription, x_wafer.visStr());
    dd4hep::SkinSurface(*fDescription, *fDetElement, "SiPMSurf_Tower"+std::to_string(fTowerNoLR), *fSipmSurf, sipmWaferVol);

    if (x_wafer.isSensitive()) {
      sipmWaferVol.setSensitiveDetector(*fSensDet);
    }

    implementSipms(sipmLayerVol);

    for (int nPhi = 0; nPhi < x_theta.nphi(); nPhi++) {
      auto towerId64 = fSegmentation->setVolumeID( fTowerNoLR, nPhi );
      int towerId32 = fSegmentation->getFirst32bits(towerId64);

      // copy number of assemblyVolume is unpredictable, use dummy volume to make use of copy number of afterwards
      dd4hep::Volume assemblyEnvelopVol( "assembly", assemblyEnvelop, fDescription->material("Vacuum") );
      fExperimentalHall->placeVolume( assemblyEnvelopVol, param->GetAssembleTransform3D(nPhi) );

      assemblyEnvelopVol.placeVolume( towerVol, towerId32, dd4hep::Position(0.,0.,-param->GetSipmHeight()/2.) );

      assemblyEnvelopVol.placeVolume( sipmLayerVol, towerId32, dd4hep::Position(0.,0.,(x_theta.height()-x_wafer.height())/2.) );

      dd4hep::PlacedVolume sipmWaferPhys = assemblyEnvelopVol.placeVolume( sipmWaferVol, towerId32, dd4hep::Position(0.,0.,(x_theta.height()+param->GetSipmHeight()-x_wafer.height())/2.) );
      sipmWaferPhys.addPhysVolID("eta", fTowerNoLR);
      sipmWaferPhys.addPhysVolID("phi", nPhi);
      sipmWaferPhys.addPhysVolID("module", 0);
      sipmWaferPhys.addPhysVolID("xmax",fNumx);
      sipmWaferPhys.addPhysVolID("ymax",fNumy);
    }
  }

  param->filled();
  param->SetTotTowerNum( towerNo - x_theta.start() );
}

void ddDRcalo::DRconstructor::implementFibers(xml_comp_t& x_theta, dd4hep::Volume& towerVol, dd4hep::Trap& trap, dd4hep::DDSegmentation::DRparamBase* param) {
  dd4hep::Tube fiber = dd4hep::Tube(0.,fX_cladC.rmax(),x_theta.height()/2.);
  dd4hep::Tube fiberC = dd4hep::Tube(0.,fX_coreC.rmin(),x_theta.height()/2.);
  dd4hep::Tube fiberS = dd4hep::Tube(0.,fX_coreS.rmin(),x_theta.height()/2.);
  auto rootTrap = trap.access();

  float sipmSize = fX_dim.dx();
  float gridSize = fX_dim.distance();
  float towerHeight = x_theta.height();

  float diff = fX_cladC.rmax(); // can be arbitrary small number
  float z1 = towerHeight/2.-2*diff; // can be arbitrary number slightly smaller than towerHeight/2-diff

  fNumx = static_cast<int>( std::floor( ( param->GetBl2()*2. - sipmSize )/gridSize ) ) + 1; // in eta direction
  fNumy = static_cast<int>( std::floor( ( param->GetH2()*2. - sipmSize )/gridSize ) ) + 1; // in phi direction

  // get normals to each side
  double norm1[3] = {0.,0.,0.}, norm2[3] = {0.,0.,0.}, norm3[3] = {0.,0.,0.}, norm4[3] = {0.,0.,0.};
  getNormals(rootTrap,z1,norm1,norm2,norm3,norm4);

  for (int row = 0; row < fNumy; row++) {
    for (int column = 0; column < fNumx; column++) {
      auto localPosition = fSegmentation->localPosition(fNumx,fNumy,column,row);
      dd4hep::Position pos = dd4hep::Position(localPosition);

      if ( std::abs(pos.x()) + fX_cladC.rmax() < rootTrap->GetBl1() && std::abs(pos.y()) + fX_cladC.rmax() < rootTrap->GetH1() ) {
        implementFiber(towerVol, pos, column, row, fiber, fiberC, fiberS); // full length fiber
      } else {
        double* normX = nullptr;
        double* normY = nullptr;

        // select two closest orthogonal sides
        if (column > fNumx/2) normX = norm2;
        else normX = norm4;

        if (row > fNumy/2) normY = norm3;
        else normY = norm1;

        // compare and choose the shortest fiber length
        float cand1 = calculateFiberLen(rootTrap, pos, normX, z1, diff, towerHeight);
        float cand2 = calculateFiberLen(rootTrap, pos, normY, z1, diff, towerHeight);
        float fiberLen = std::min(cand1,cand2);

        // trim fiber length in the case calculated length is longer than tower height
        if (fiberLen > towerHeight) fiberLen = towerHeight;
        float centerZ = towerHeight/2. - fiberLen/2.;

        // final check
        checkContained(rootTrap,pos,towerHeight/2.-fiberLen);

        dd4hep::Position centerPos( pos.x(),pos.y(),centerZ );

        dd4hep::Tube shortFiber = dd4hep::Tube(0.,fX_cladC.rmax(),fiberLen/2.);
        dd4hep::Tube shortFiberC = dd4hep::Tube(0.,fX_coreC.rmin(),fiberLen/2.);
        dd4hep::Tube shortFiberS = dd4hep::Tube(0.,fX_coreS.rmin(),fiberLen/2.);

        implementFiber(towerVol, centerPos, column, row, shortFiber, shortFiberC, shortFiberS);
      }
    }
  }
}

void ddDRcalo::DRconstructor::implementFiber(dd4hep::Volume& towerVol, dd4hep::Position& pos, int col, int row,
                                              dd4hep::Tube& fiber, dd4hep::Tube& fiberC, dd4hep::Tube& fiberS) {
  dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
  dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

  auto fiberId64 = fSegmentation->setCellID(fTowerNoLR, 0, fNumx, fNumy, col, row);
  int fiberId32 = fSegmentation->getLast32bits(fiberId64);

  if ( fSegmentation->IsCerenkov(col,row) ) { //c fibre
    dd4hep::Volume cladVol("cladC", fiber, fDescription->material(fX_cladC.materialStr()));
    if (fVis) cladVol.setVisAttributes(*fDescription, fX_cladC.visStr()); // high CPU consumption!
    towerVol.placeVolume( cladVol, fiberId32, trans );

    dd4hep::Volume coreVol("coreC", fiberC, fDescription->material(fX_coreC.materialStr()));
    if (fVis) coreVol.setVisAttributes(*fDescription, fX_coreC.visStr());
    cladVol.placeVolume( coreVol, fiberId32 );

    coreVol.setRegion(*fDescription, fX_det.regionStr());
    cladVol.setRegion(*fDescription, fX_det.regionStr());
  } else { // s fibre
    dd4hep::Volume cladVol("cladS", fiber, fDescription->material(fX_coreC.materialStr()));
    if (fVis) cladVol.setVisAttributes(*fDescription, fX_coreC.visStr());
    towerVol.placeVolume( cladVol, fiberId32, trans );

    dd4hep::Volume coreVol("coreS", fiberS, fDescription->material(fX_coreS.materialStr()));
    if (fVis) coreVol.setVisAttributes(*fDescription, fX_coreS.visStr());
    cladVol.placeVolume( coreVol, fiberId32 );

    coreVol.setRegion(*fDescription, fX_det.regionStr());
    cladVol.setRegion(*fDescription, fX_det.regionStr());
  }
}

void ddDRcalo::DRconstructor::implementSipms(dd4hep::Volume& sipmLayerVol) {
  xml_comp_t x_glass ( fX_sipmDim.child( _Unicode(sipmGlass) ) );
  xml_comp_t x_wafer ( fX_sipmDim.child( _Unicode(sipmWafer) ) );
  xml_comp_t x_filter ( fX_sipmDim.child( _Unicode(filter) ) );

  float sipmSize = fX_dim.dx();
  double windowHeight = fX_sipmDim.height() - x_filter.height() - x_wafer.height();

  // Glass box
  dd4hep::Box sipmEnvelop(sipmSize/2., sipmSize/2., windowHeight/2.);
  dd4hep::Volume sipmEnvelopVol( "sipmEnvelop", sipmEnvelop, fDescription->material(x_glass.materialStr()) );
  if (fVis) sipmEnvelopVol.setVisAttributes(*fDescription, fX_sipmDim.visStr());

  // Kodak filter
  dd4hep::Box filterBox( sipmSize/2., sipmSize/2., x_filter.height()/2. );
  dd4hep::Volume filterVol( "filter", filterBox, fDescription->material(x_filter.materialStr()) );
  if (fVis) filterVol.setVisAttributes(*fDescription, x_filter.visStr());
  dd4hep::SkinSurface(*fDescription, *fDetElement, "FilterSurf_Tower"+std::to_string(fTowerNoLR), *fFilterSurf, filterVol);

  // dummy Box placed at C channel (instead of Kodak filter at S channel)
  dd4hep::Box dummyBox( sipmSize/2., sipmSize/2., x_filter.height()/2. );
  dd4hep::Volume dummyVol( "dummy", dummyBox, fDescription->material(x_glass.materialStr()) );
  if (fVis) dummyVol.setVisAttributes(*fDescription, fX_sipmDim.visStr());

  int sipmNo = 0;
  for (int row = 0; row < fNumy; row++, sipmNo++) {
    for (int column = 0; column < fNumx; column++, sipmNo++) {
      auto localPosition = fSegmentation->localPosition(fNumx,fNumy,column,row);
      auto sipmId64 = fSegmentation->setCellID(fTowerNoLR, 0, fNumx, fNumy, column, row);
      int sipmId32 = fSegmentation->getLast32bits(sipmId64);

      dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
      dd4hep::Position pos = dd4hep::Position(localPosition.x(),localPosition.y(),x_filter.height()/2.);
      dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

      sipmLayerVol.placeVolume( sipmEnvelopVol, trans );

      if ( !fSegmentation->IsCerenkov(column,row) ) { //s channel
        dd4hep::Position posFilter = dd4hep::Position(localPosition.x(),localPosition.y(),-windowHeight/2.);
        dd4hep::Transform3D transFilter = dd4hep::Transform3D(rot,posFilter);

        sipmLayerVol.placeVolume( filterVol, transFilter );
      } else { // c channel
        dd4hep::Position posDummy = dd4hep::Position(localPosition.x(),localPosition.y(),-windowHeight/2.);
        dd4hep::Transform3D transDummy = dd4hep::Transform3D(rot,posDummy);

        sipmLayerVol.placeVolume( dummyVol, sipmId32, transDummy );
      }
    }
  }
}

double ddDRcalo::DRconstructor::calculateDistAtZ(TGeoTrap* rootTrap, dd4hep::Position& pos, double* norm, double z) {
  double pos_[3] = {pos.x(),pos.y(),z};

  checkContained(rootTrap,pos,z);

  return rootTrap->DistFromInside(pos_,norm);
}

float ddDRcalo::DRconstructor::calculateFiberLen(TGeoTrap* rootTrap, dd4hep::Position& pos, double* norm, double z1, double diff, double towerHeight) {
  float z2 = z1+diff;
  float y1 = calculateDistAtZ(rootTrap,pos,norm,z1);
  float y2 = calculateDistAtZ(rootTrap,pos,norm,z2);
  float slope = (y2-y1)/diff;
  float y0 = (y1*z2-y2*z1)/diff;
  float z = (fX_cladC.rmax()-y0)/slope;
  float fiberLen = towerHeight/2. - z;

  return fiberLen;
}

void ddDRcalo::DRconstructor::checkContained(TGeoTrap* rootTrap, dd4hep::Position& pos, double z) {
  double pos_[3] = {pos.x(),pos.y(),z};

  if (!rootTrap->Contains(pos_)) throw std::runtime_error("Fiber must be in the tower!");
}

void ddDRcalo::DRconstructor::getNormals(TGeoTrap* rootTrap, double z, double* norm1, double* norm2, double* norm3, double* norm4) {
  dd4hep::Position pos1 = dd4hep::Position( fSegmentation->localPosition(fNumx,fNumy,fNumx/2,0) );
  dd4hep::Position pos2 = dd4hep::Position( fSegmentation->localPosition(fNumx,fNumy,fNumx-1,fNumy/2) );
  dd4hep::Position pos3 = dd4hep::Position( fSegmentation->localPosition(fNumx,fNumy,fNumx/2,fNumy-1) );
  dd4hep::Position pos4 = dd4hep::Position( fSegmentation->localPosition(fNumx,fNumy,0,fNumy/2) );
  double pos1_[3] = {pos1.x(),pos1.y(),z};
  double pos2_[3] = {pos2.x(),pos2.y(),z};
  double pos3_[3] = {pos3.x(),pos3.y(),z};
  double pos4_[3] = {pos4.x(),pos4.y(),z};
  double dir[3] = {0.,0.,0.};

  rootTrap->ComputeNormal(pos1_,dir,norm1);
  rootTrap->ComputeNormal(pos2_,dir,norm2);
  rootTrap->ComputeNormal(pos3_,dir,norm3);
  rootTrap->ComputeNormal(pos4_,dir,norm4);
  norm1[2] = 0.; // check horizontal distance only
  norm2[2] = 0.;
  norm3[2] = 0.;
  norm4[2] = 0.;
}
