#ifndef DRsimFilterParameterisation_h
#define DRsimFilterParameterisation_h 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include <vector>

class DRsimFilterParameterisation : public G4VPVParameterisation {
public:
  DRsimFilterParameterisation(const G4int numx, const G4int numy, G4Material* filterMat);
  virtual ~DRsimFilterParameterisation();

  virtual void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;
  virtual G4Material* ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable* parentTouch);

private:
  std::vector<G4double> fXFilter;
  std::vector<G4double> fYFilter;
  G4int fNumx;
  G4int fNumy;
  G4Material* fFilterMat;
};

#endif
