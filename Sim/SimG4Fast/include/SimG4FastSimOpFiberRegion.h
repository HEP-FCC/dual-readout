#ifndef SimG4FastSimOpFiberRegion_h
#define SimG4FastSimOpFiberRegion_h 1

#include <vector>
#include <iostream>
#include <memory>

#include "FastSimModelOpFiber.h"

#include "G4LogicalVolume.hh"

// Geant
class G4VFastSimulationModel;
class G4Region;

class SimG4FastSimOpFiberRegion {
public:
  explicit SimG4FastSimOpFiberRegion();
  virtual ~SimG4FastSimOpFiberRegion();
  
  void create();

private:
  FastSimModelOpFiber* fModel;
};

#endif
