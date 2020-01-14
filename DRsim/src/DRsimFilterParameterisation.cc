#include "RecoInterface.h"
#include "DRsimFilterParameterisation.hh"
#include "DRsimCellParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DRsimFilterParameterisation::DRsimFilterParameterisation(const G4int numx, const G4int numy)
: G4VPVParameterisation()
{
  for (G4int copyNo=0;copyNo<numx*numy;copyNo++) {
    G4int column = copyNo / numy;
    G4int row = copyNo % numy;

    if ( !RecoInterface::IsCerenkov(column,row) ) {
      fXFilter.push_back( (column-numx/2)*1.5*mm + ( numx%2==0 ? 0.75*mm : 0 ) );
      fYFilter.push_back( (row-numy/2)*1.5*mm + ( numy%2==0 ? 0.75*mm : 0 ) );
    }
  }
  fNumx = numx;
  fNumy = numy;
}

DRsimFilterParameterisation::~DRsimFilterParameterisation() {}

void DRsimFilterParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const {
  physVol->SetTranslation(G4ThreeVector(fXFilter[copyNo],fYFilter[copyNo],0.));
}
