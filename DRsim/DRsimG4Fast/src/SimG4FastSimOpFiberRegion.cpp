#include "SimG4FastSimOpFiberRegion.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"
#include "G4VFastSimulationModel.hh"

DECLARE_COMPONENT(SimG4FastSimOpFiberRegion)

SimG4FastSimOpFiberRegion::SimG4FastSimOpFiberRegion(const std::string& type, const std::string& name, const IInterface* parent)
: AlgTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
}

SimG4FastSimOpFiberRegion::~SimG4FastSimOpFiberRegion() {}

StatusCode SimG4FastSimOpFiberRegion::initialize() {
  if (AlgTool::initialize().isFailure())
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimOpFiberRegion::finalize() { return AlgTool::finalize(); }

StatusCode SimG4FastSimOpFiberRegion::create() {
  auto* regionStore = G4RegionStore::GetInstance();
  auto* region = regionStore->GetRegion( static_cast<std::string>(m_regionName) );

  m_model = std::make_unique<FastSimModelOpFiber>("FastSimModelOpFiber",region);

  info() << "Creating FastSimModelOpFiber model with the region " << m_regionName << endmsg;

  return StatusCode::SUCCESS;
}
