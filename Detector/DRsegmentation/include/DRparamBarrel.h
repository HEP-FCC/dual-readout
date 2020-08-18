#ifndef DRparamBarrel_h
#define DRparamBarrel_h 1

#include "TVector3.h"
#include "DD4hep/DetFactoryHelper.h"

#include <vector>
#include <cmath>

namespace ddDRcalo {
  class DRparamBarrel {
  public:
    DRparamBarrel();
    ~DRparamBarrel();

    void SetIsRHS(bool isRHS) { fIsRHS = isRHS; }
    void SetInnerX(double innerX) { fInnerX = innerX; }
    void SetTowerH(double towerH) { fTowerH = towerH; }
    void SetNumZRot(int num) { fNumZRot = num; fPhiZRot = 2*M_PI/(double)num; }
    void SetDeltaTheta(double theta) { fDeltaTheta = theta; }
    void SetThetaOfCenter(double theta) { fThetaOfCenter = theta; }
    void SetSipmHeight(double SipmHeight) { fSipmHeight = SipmHeight; }

    bool GetIsRHS() { return fIsRHS; }
    double GetCurrentInnerR() { return fCurrentInnerR; }
    double GetSipmHeight() { return fSipmHeight; }
    TVector3 GetV1() { return fV1; }
    TVector3 GetV2() { return fV2; }
    TVector3 GetV3() { return fV3; }
    TVector3 GetV4() { return fV4; }
    double GetH1() { return fCurrentInnerHalf; }
    double GetBl1() { return fV3.X()*std::tan(fPhiZRot/2.); }
    double GetTl1() { return fV1.X()*std::tan(fPhiZRot/2.); }
    double GetH2() { return fCurrentOuterHalf; }
    double GetBl2() { return fV4.X()*std::tan(fPhiZRot/2.); }
    double GetTl2() { return fV2.X()*std::tan(fPhiZRot/2.); }

    double GetH2sipm() { return fCurrentOuterHalfSipm; }
    double GetBl2sipm() { return fV4sipm.X()*std::tan(fPhiZRot/2.); }
    double GetTl2sipm() { return fV2sipm.X()*std::tan(fPhiZRot/2.); }

    dd4hep::RotationZYX GetRotationZYX(int numPhi);
    dd4hep::Position GetTowerPos(int numPhi);
    dd4hep::Position GetAssemblePos(int numPhi);
    dd4hep::Position GetSipmLayerPos(int numPhi);

    dd4hep::Transform3D GetTransform3D(int numPhi);
    dd4hep::Transform3D GetAssembleTransform3D(int numPhi);
    dd4hep::Transform3D GetSipmTransform3D(int numPhi);

    int signedTowerNo(int unsignedTowerNo) { return fIsRHS ? unsignedTowerNo : -unsignedTowerNo-1; }

    void init();
    void filled() { fFilled = true; }

    static DRparamBarrel* GetInstance();

  private:
    bool fIsRHS;
    double fPhiZRot;
    double fInnerX;
    double fTowerH;
    int fNumZRot;
    double fDeltaTheta;
    double fThetaOfCenter;
    double fCurrentInnerR;
    TVector3 fCurrentCenter;
    TVector3 fV1;
    TVector3 fV2;
    TVector3 fV3;
    TVector3 fV4;
    TVector3 fV2sipm;
    TVector3 fV4sipm;
    double fSipmHeight;

    double fInnerY;
    double fCurrentInnerHalf;
    double fCurrentOuterHalf;
    double fCurrentOuterHalfSipm;

    static DRparamBarrel* fInstance;

    std::vector<double> fDeltaThetaVec;
    std::vector<double> fThetaOfCenterVec;
    bool fFilled;
  };
}

#endif
