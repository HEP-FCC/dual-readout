#include "SimG4DRcaloActions.h"

DECLARE_COMPONENT(SimG4DRcaloActions)

SimG4DRcaloActions::SimG4DRcaloActions(const std::string& type, const std::string& name, const IInterface* parent)
: AlgTool(type, name, parent), m_geoSvc("GeoSvc", name) {
  declareInterface<ISimG4ActionTool>(this);
}

SimG4DRcaloActions::~SimG4DRcaloActions() {}

StatusCode SimG4DRcaloActions::initialize() {
  if (AlgTool::initialize().isFailure())
    return StatusCode::FAILURE;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  return StatusCode::SUCCESS;
}

StatusCode SimG4DRcaloActions::finalize() { return AlgTool::finalize(); }

G4VUserActionInitialization* SimG4DRcaloActions::userActionInitialization() {
  auto* actions = new drc::SimG4DRcaloActionInitialization();
  actions->setSegmentation(pSeg);
  actions->setBirksConstant(m_scintName,m_birks);

  return actions;
}
