#ifndef dimensionE_h
#define dimensionE_h 1

#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "globals.hh"
#include <string.h>
#include <vector>

using namespace std;

class dimensionE {
public:
  dimensionE();
  ~dimensionE();

  void Rbool(G4bool Rbool) { fRbool = Rbool; }
  void SetInnerR_new(G4double innerR) { finnerR_new = innerR; }
  void SetTower_height(G4double tower_height) { ftower_height = tower_height; }
  void SetNumZRot(G4int num) { fnumzrot = num; fPhiZRot = 2*M_PI/(G4double)num; }
  void SetDeltaTheta(G4double theta) { fdeltatheta = theta; }
  void SetThetaOfCenter(G4double theta) { fthetaofcenter = theta; }
  void SetPMTT(G4double PMTT) { fPMTT = PMTT; }

  bool GetRbool() { return fRbool; }

  void CalBasic();

  G4double GetInnerR_new();
  G4double GetTrns_Length();
  G4ThreeVector GetTrns_Vector();
  G4ThreeVector GetV1();
  G4ThreeVector GetV2();
  G4ThreeVector GetV3();
  G4ThreeVector GetV4();

  G4ThreeVector GetOrigin(G4int i);
  void Getpt(G4ThreeVector *pt);

  G4ThreeVector GetOrigin_PMTG(G4int i);
  void Getpt_PMTG(G4ThreeVector *pt);

  G4RotationMatrix* GetRM(G4int i);

private:
  G4double fPhiZRot;
  G4bool fRbool;
  G4bool fcalbasicbool;
  G4double ftower_height;
  G4double fnumzrot;
  G4double fdeltatheta;
  G4double fthetaofcenter;
  G4double finnerR_new;
  G4double fTrns_Length;
  G4ThreeVector fTrns_Vector;
  G4ThreeVector fV1;
  G4ThreeVector fV2;
  G4ThreeVector fV3;
  G4ThreeVector fV4;
  G4double fPMTT;

  G4double x,y,z;

  G4double innerSide_half;
  G4double outerSide_half;
  G4RotationMatrix* RotMatrix;

protected:
};

#endif
