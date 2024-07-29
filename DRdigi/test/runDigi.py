from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import k4DataSvc
dataservice = k4DataSvc("EventDataSvc", input="sim.root")

from Configurables import PodioInput
podioinput = PodioInput("PodioInput", collections = ["RawTimeStructs", "RawCalorimeterHits", "SimCalorimeterHits", "Sim3dCalorimeterHits", "RawWavlenStructs", "GenParticles", "Leakages"], OutputLevel = DEBUG)

from Configurables import DigiSiPM
digi = DigiSiPM("DigiSiPM", OutputLevel=DEBUG)

from Configurables import PodioOutput
podiooutput = PodioOutput("PodioOutput", filename = "digi.root", OutputLevel = DEBUG)
podiooutput.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [
        podioinput,
        digi,
        podiooutput
    ],
    EvtSel = 'NONE',
    EvtMax = 10,
    ExtSvc = [dataservice]
)
