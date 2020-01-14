#ifndef DRsimCellParameterisation_h
#define DRsimCellParameterisation_h 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include "G4VisAttributes.hh"
#include <vector>
class G4VPhysicalVolume;

class DRsimCellParameterisation : public G4VPVParameterisation {
public:
  DRsimCellParameterisation(const G4int numx, const G4int numy);
  virtual ~DRsimCellParameterisation();

  virtual void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;

private:
  std::vector<G4double> fXCell;
  std::vector<G4double> fYCell;
  G4int fNumx;
  G4int fNumy;
};

#endif
