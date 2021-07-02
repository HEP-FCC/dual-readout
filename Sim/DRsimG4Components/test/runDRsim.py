from Gaudi.Configuration import *
from Configurables import ApplicationMgr
from GaudiKernel import SystemOfUnits as units

from Configurables import k4DataSvc
dataservice = k4DataSvc("EventDataSvc")

from Configurables import GenAlg, MomentumRangeParticleGun
pgun = MomentumRangeParticleGun("PGun",
  PdgCodes=[11], # electron
  MomentumMin = 20.*units.GeV, # GeV
  MomentumMax = 20.*units.GeV, # GeV
  ThetaMin = 1.5335, # rad
  ThetaMax = 1.5335, # rad
  PhiMin = 0.01745, # rad
  PhiMax = 0.01745 # rad
)

from Configurables import FlatSmearVertex
smearTool = FlatSmearVertex("VertexSmearingTool",
  yVertexMin = -36.42, # mm
  yVertexMax = -26.42, # mm
  zVertexMin = -52.135, # mm
  zVertexMax = -42.135, # mm
  beamDirection = 0 # 1, 0, -1
)

from Configurables import HepMCToEDMConverter
hepmc2edm = HepMCToEDMConverter("Converter")

gen = GenAlg("ParticleGun", SignalProvider=pgun, VertexSmearingTool=smearTool)

from Configurables import GeoSvc
geoservice = GeoSvc(
  "GeoSvc",
  detectors = [
    'file:share/compact/DRcalo.xml'
  ]
)

from Configurables import SimG4Svc, SimG4FastSimPhysicsList, SimG4FastSimOpFiberRegion, SimG4OpticalPhysicsList
regionTool = SimG4FastSimOpFiberRegion("fastfiber")
opticalPhysicsTool = SimG4OpticalPhysicsList("opticalPhysics", fullphysics="SimG4FtfpBert")
physicslistTool = SimG4FastSimPhysicsList("Physics", fullphysics=opticalPhysicsTool)

from Configurables import SimG4DRcaloActions
actionTool = SimG4DRcaloActions("SimG4DRcaloActions")

# Name of the tool in GAUDI is "XX/YY" where XX is the tool class name and YY is the given name
geantservice = SimG4Svc("SimG4Svc",
  physicslist = physicslistTool,
  regions = ["SimG4FastSimOpFiberRegion/fastfiber"],
  actions = actionTool
)

from Configurables import SimG4Alg, SimG4PrimariesFromEdmTool
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
edmConverter = SimG4PrimariesFromEdmTool("EdmConverter")

from Configurables import SimG4SaveSmearedParticles, SimG4SaveDRcaloHits, SimG4SaveDRcaloMCTruth
savePtcTool = SimG4SaveSmearedParticles("saveSmearedParticles")
saveDRcaloTool = SimG4SaveDRcaloHits("saveDRcaloTool", readoutNames = ["DRcaloSiPMreadout"])
saveMCTruthTool = SimG4SaveDRcaloMCTruth("saveMCTruthTool") # need SimG4DRcaloActions

geantsim = SimG4Alg("SimG4Alg",
  outputs = [
    "SimG4SaveSmearedParticles/saveSmearedParticles",
    "SimG4SaveDRcaloHits/saveDRcaloTool",
    "SimG4SaveDRcaloMCTruth/saveMCTruthTool"
  ],
  eventProvider = edmConverter
)

from Configurables import PodioOutput
podiooutput = PodioOutput("PodioOutput", filename = "sim.root")
podiooutput.outputCommands = ["drop RecParticlesSmeared*", "drop SmearedParticlesToParticles*"]

ApplicationMgr(
  TopAlg = [gen, hepmc2edm, geantsim, podiooutput],
  EvtSel = 'NONE',
  EvtMax = 10,
  # order is important, as GeoSvc is needed by SimG4Svc
  ExtSvc = [dataservice, geoservice, geantservice]
)
