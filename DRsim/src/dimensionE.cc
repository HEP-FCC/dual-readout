#include "dimensionE.hh"

#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "geomdefs.hh"

#include <cmath>
#include <stdio.h>
#include <float.h>

using namespace std;

dimensionE::dimensionE() {
  fRbool = 0;
  fcalbasicbool = 0;
  ftower_height = 0;
  fnumzrot = 0;
  fdeltatheta = 0;
  fthetaofcenter = 0;
  finnerR_new = 0;
  fPhiZRot = 0;
  fV1 = G4ThreeVector();
  fV2 = G4ThreeVector();
  fV3 = G4ThreeVector();
  fV4 = G4ThreeVector();
}

dimensionE::~dimensionE() {}

void dimensionE::CalBasic() {
  fcalbasicbool = 1;
  fTrns_Length = ftower_height/2.+finnerR_new;
  fTrns_Vector =  G4ThreeVector(cos(fthetaofcenter)*fTrns_Length,0,sin(fthetaofcenter)*fTrns_Length);

  fV1 = G4ThreeVector(
    cos(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.),
    0,
    sin(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
  );

  fV2 = G4ThreeVector(
    cos(fthetaofcenter)*(finnerR_new+ftower_height)+sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.),
    0,
    sin(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)
  );

  fV3 = G4ThreeVector(
    cos(fthetaofcenter)*finnerR_new-sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.),
    0,
    sin(fthetaofcenter)*finnerR_new+sin(fthetaofcenter)*finnerR_new*tan(fdeltatheta/2.)
  );

  fV4 = G4ThreeVector(
    cos(fthetaofcenter)*(finnerR_new+ftower_height)-sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.),
    0,
    sin(fthetaofcenter)*(finnerR_new+ftower_height)+sin(fthetaofcenter)*(finnerR_new+ftower_height)*tan(fdeltatheta/2.)
  );

}

G4double dimensionE::GetInnerR_new() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return 0;
  } else return finnerR_new;
}

G4double dimensionE::GetTrns_Length() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return 0;
  } else return fTrns_Length;
}

G4ThreeVector dimensionE::GetTrns_Vector() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else return fTrns_Vector;
}

G4ThreeVector dimensionE::GetV1() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else return fV1;
}

G4ThreeVector dimensionE::GetV2() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else return fV2;
}

G4ThreeVector dimensionE::GetV3() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else return fV3;
}

G4ThreeVector dimensionE::GetV4() {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else return fV4;
}

G4ThreeVector dimensionE::GetOrigin(G4int i) {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else {
    if(fRbool==1) {
      x=cos(i*fPhiZRot)*fTrns_Vector.getX();
      y=sin(i*fPhiZRot)*fTrns_Vector.getX();
      z=fTrns_Vector.getZ();

      return G4ThreeVector(x,y,z);
    } else {
      x=cos(i*fPhiZRot)*fTrns_Vector.getX();
      y=sin(i*fPhiZRot)*fTrns_Vector.getX();
      z=-fTrns_Vector.getZ();

      return G4ThreeVector(x,y,z);
    }
  }
}

G4ThreeVector dimensionE::GetOrigin_PMTG(G4int i) {
  if(fcalbasicbool==0) {
    cout<<"fcalbasicbool = 0"<<endl;
    return G4ThreeVector();
  } else {
    if(fRbool==1) {
      x=cos(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();
      y=sin(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();
      z=fTrns_Vector.getZ()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();

      return G4ThreeVector(x,y,z);
    } else {
      x=cos(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();
      y=sin(i*fPhiZRot)*fTrns_Vector.getX()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();
      z=-fTrns_Vector.getZ()*(fTrns_Vector.mag()+ftower_height/2.+fPMTT/2.)/fTrns_Vector.mag();

      return G4ThreeVector(x,y,z);
    }
  }
}

void dimensionE::Getpt(G4ThreeVector *pt) {
  innerSide_half = finnerR_new*tan(fdeltatheta/2.);
  outerSide_half= (finnerR_new+ftower_height)*tan(fdeltatheta/2.);

  if(fRbool == 1) {
    pt[0]=G4ThreeVector(-(fV3.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
    pt[1]=G4ThreeVector((fV3.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
    pt[2]=G4ThreeVector(-(fV1.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
    pt[3]=G4ThreeVector((fV1.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
    pt[4]=G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
    pt[5]=G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
    pt[6]=G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
    pt[7]=G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
  } else {
    pt[0]=G4ThreeVector(-(fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
    pt[1]=G4ThreeVector((fV1.getX()*tan(fPhiZRot/2.)),-innerSide_half,-ftower_height/2.);
    pt[2]=G4ThreeVector(-(fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
    pt[3]=G4ThreeVector((fV3.getX()*tan(fPhiZRot/2.)),innerSide_half,-ftower_height/2.);
    pt[4]=G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
    pt[5]=G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,ftower_height/2.);
    pt[6]=G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
    pt[7]=G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,ftower_height/2.);
  }
}

void dimensionE::Getpt_PMTG(G4ThreeVector *pt) {
  innerSide_half = finnerR_new*tan(fdeltatheta/2.);
  outerSide_half= (finnerR_new+ftower_height)*tan(fdeltatheta/2.);

  if(fRbool == 1) {
    pt[0] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/2.);
    pt[1] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/2.);
    pt[2] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/2.);
    pt[3] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/2.);
    pt[4] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/2.);
    pt[5] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/2.);
    pt[6] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/2.);
    pt[7] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/2.);
  } else {
    pt[0] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/2.);
    pt[1] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,-fPMTT/2.);
    pt[2] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/2.);
    pt[3] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,-fPMTT/2.);
    pt[4] = G4ThreeVector(-(fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/2.);
    pt[5] = G4ThreeVector((fV2.getX()*tan(fPhiZRot/2.)),-outerSide_half,fPMTT/2.);
    pt[6] = G4ThreeVector(-(fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/2.);
    pt[7] = G4ThreeVector((fV4.getX()*tan(fPhiZRot/2.)),outerSide_half,fPMTT/2.);
  }
}

G4RotationMatrix* dimensionE::GetRM(G4int i) {
  if(fRbool==1) {
    RotMatrix = new G4RotationMatrix();

    RotMatrix->rotateZ(M_PI/2.);
    RotMatrix->rotateZ(-i*fPhiZRot);
    RotMatrix->rotateX(M_PI/2.);
    RotMatrix->rotateX(-fthetaofcenter);

    return RotMatrix;
  } else {
    RotMatrix = new G4RotationMatrix();

    RotMatrix->rotateZ(M_PI/2.);
    RotMatrix->rotateZ(-i*fPhiZRot);
    RotMatrix->rotateX(M_PI/2.);
    RotMatrix->rotateX(fthetaofcenter);

    return RotMatrix;
  }
}
