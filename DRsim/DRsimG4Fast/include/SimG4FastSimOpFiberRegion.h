#ifndef SimG4FastSimOpFiberRegion_h
#define SimG4FastSimOpFiberRegion_h 1

#include <vector>
#include <iostream>
#include <memory>

#include "FastSimModelOpFiber.h"

#include "G4LogicalVolume.hh"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "k4Interface/ISimG4RegionTool.h"

// Geant
class G4VFastSimulationModel;
class G4Region;

class SimG4FastSimOpFiberRegion : public AlgTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4FastSimOpFiberRegion(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FastSimOpFiberRegion();

  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual StatusCode create() final;

private:
  std::unique_ptr<FastSimModelOpFiber> m_model;

  Gaudi::Property<std::string> m_regionName{this, "regionName", "FastSimOpFiberRegion", "fast fiber region name"};
};

#endif
