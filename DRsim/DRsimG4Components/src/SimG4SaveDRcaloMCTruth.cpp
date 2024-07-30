#include "SimG4SaveDRcaloMCTruth.h"

#include "G4RunManager.hh"

DECLARE_COMPONENT(SimG4SaveDRcaloMCTruth)

SimG4SaveDRcaloMCTruth::SimG4SaveDRcaloMCTruth(const std::string& aType, const std::string& aName, const IInterface* aParent)
: AlgTool(aType, aName, aParent), m_geantSvc("SimG4Svc", aName) {
  declareInterface<ISimG4SaveOutputTool>(this);
}

SimG4SaveDRcaloMCTruth::~SimG4SaveDRcaloMCTruth() {}

StatusCode SimG4SaveDRcaloMCTruth::initialize() {
  if (AlgTool::initialize().isFailure())
    return StatusCode::FAILURE;

  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }

  auto* runManager = G4RunManager::GetRunManager();
  const auto* eventAction = dynamic_cast<const drc::SimG4DRcaloEventAction*>(runManager->GetUserEventAction());

  if (!eventAction) {
    error() << "Unable to cast to SimG4DRcaloEventAction from G4UserEventAction" << endmsg;
    return StatusCode::FAILURE;
  }

  m_eventAction = const_cast<drc::SimG4DRcaloEventAction*>(eventAction); // HACK!!!

  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveDRcaloMCTruth::finalize() { return AlgTool::finalize(); }

StatusCode SimG4SaveDRcaloMCTruth::saveOutput(const G4Event&) {
  auto* edeps = m_eventAction->getEdepsCollection();
  auto* edeps3d = m_eventAction->getEdeps3dCollection();
  auto* leakages = m_eventAction->getLeakagesCollection();

  m_Edeps.put(edeps);
  m_Edeps3d.put(edeps3d);
  m_Leakages.put(leakages);

  return StatusCode::SUCCESS;
}
