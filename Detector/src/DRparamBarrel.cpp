#include "DRparamBarrel.h"

#include "Math/GenVector/RotationZYX.h"

ddDRcalo::DRparamBarrel::DRparamBarrel() {
  fIsRHS = 0;
  fPhiZRot = 0.;
  fInnerX = 0.;
  fTowerH = 0.;
  fNumZRot = 0;
  fDeltaTheta = 0.;
  fThetaOfCenter = 0.;
  fCurrentInnerR = 0.;
  fPhiZRot = 0;
  fCurrentCenter = TVector3();
  fV1 = TVector3();
  fV2 = TVector3();
  fV3 = TVector3();
  fV4 = TVector3();
  fPMTT = 0.;
  fInnerY = 0.;
  fCurrentInnerHalf = 0.;
  fCurrentOuterHalf = 0.;
}

ddDRcalo::DRparamBarrel::~DRparamBarrel() {}

void ddDRcalo::DRparamBarrel::init() {
  fCurrentInnerR = fInnerX/std::cos(fThetaOfCenter);
  double trnsLength = fTowerH/2.+fCurrentInnerR;
  fCurrentCenter =  TVector3(std::cos(fThetaOfCenter)*trnsLength,0.,std::sin(fThetaOfCenter)*trnsLength);

  fInnerY = 2*fInnerX*std::tan(M_PI/(double)fNumZRot);
  fCurrentInnerHalf = fCurrentInnerR*std::tan(fDeltaTheta/2.);
  fCurrentOuterHalf = (fCurrentInnerR+fTowerH)*std::tan(fDeltaTheta/2.);

  fV1 = TVector3(
    std::cos(fThetaOfCenter)*fCurrentInnerR+std::sin(fThetaOfCenter)*fCurrentInnerR*std::tan(fDeltaTheta/2.),
    0.,
    std::sin(fThetaOfCenter)*fCurrentInnerR-std::cos(fThetaOfCenter)*fCurrentInnerR*std::tan(fDeltaTheta/2.)
  );

  fV2 = TVector3(
    std::cos(fThetaOfCenter)*(fCurrentInnerR+fTowerH)+std::sin(fThetaOfCenter)*(fCurrentInnerR+fTowerH)*std::tan(fDeltaTheta/2.),
    0.,
    std::sin(fThetaOfCenter)*(fCurrentInnerR+fTowerH)-std::cos(fThetaOfCenter)*(fCurrentInnerR+fTowerH)*std::tan(fDeltaTheta/2.)
  );

  fV3 = TVector3(
    std::cos(fThetaOfCenter)*fCurrentInnerR-std::sin(fThetaOfCenter)*fCurrentInnerR*std::tan(fDeltaTheta/2.),
    0.,
    std::sin(fThetaOfCenter)*fCurrentInnerR+std::cos(fThetaOfCenter)*fCurrentInnerR*std::tan(fDeltaTheta/2.)
  );

  fV4 = TVector3(
    std::cos(fThetaOfCenter)*(fCurrentInnerR+fTowerH)-std::sin(fThetaOfCenter)*(fCurrentInnerR+fTowerH)*std::tan(fDeltaTheta/2.),
    0.,
    std::sin(fThetaOfCenter)*(fCurrentInnerR+fTowerH)+std::cos(fThetaOfCenter)*(fCurrentInnerR+fTowerH)*std::tan(fDeltaTheta/2.)
  );
}

TVector3 ddDRcalo::DRparamBarrel::GetTowerCenter(int numPhi) {
  double numPhi_ = (double)numPhi;

  double x = std::cos(numPhi_*fPhiZRot)*fCurrentCenter.X();
  double y = std::sin(numPhi_*fPhiZRot)*fCurrentCenter.X();
  double z = fIsRHS ? fCurrentCenter.Z() : -fCurrentCenter.Z();

  return TVector3(x,y,z);
}

TVector3 ddDRcalo::DRparamBarrel::GetSipmLayerCenter(int numPhi) {
  double numPhi_ = (double)numPhi;

  double x = std::cos(numPhi_*fPhiZRot)*fCurrentCenter.X()*(fCurrentCenter.Mag()+fTowerH/2.+fPMTT/2.)/fCurrentCenter.Mag();
  double y = std::sin(numPhi_*fPhiZRot)*fCurrentCenter.X()*(fCurrentCenter.Mag()+fTowerH/2.+fPMTT/2.)/fCurrentCenter.Mag();
  double z_abs = fCurrentCenter.Z()*(fCurrentCenter.Mag()+fTowerH/2.+fPMTT/2.)/fCurrentCenter.Mag();
  double z = fIsRHS ? z_abs : -z_abs;

  return TVector3(x,y,z);
}

dd4hep::Transform3D ddDRcalo::DRparamBarrel::GetTransform3D(int numPhi) {
  double numPhi_ = (double)numPhi;
  double xRot = fIsRHS ? -fThetaOfCenter : fThetaOfCenter;
  double zRot = fIsRHS ? -M_PI/2. : M_PI/2.;
  dd4hep::RotationZYX rot = dd4hep::RotationZYX(zRot, M_PI/2.+xRot, 0.);
  ROOT::Math::RotationZ rotZ = ROOT::Math::RotationZ(numPhi_*fPhiZRot);
  rot = rotZ*rot;

  double x = std::cos(numPhi_*fPhiZRot)*fCurrentCenter.X();
  double y = std::sin(numPhi_*fPhiZRot)*fCurrentCenter.X();
  double z = fIsRHS ? fCurrentCenter.Z() : -fCurrentCenter.Z();
  dd4hep::Position pos = dd4hep::Position(x,y,z);

  return dd4hep::Transform3D(rot,pos);
}
