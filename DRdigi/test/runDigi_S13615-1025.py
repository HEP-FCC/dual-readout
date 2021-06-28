from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import k4DataSvc
dataservice = k4DataSvc("EventDataSvc", input="sim.root")

from Configurables import PodioInput
podioinput = PodioInput("PodioInput", collections = ["RawTimeStructs", "RawCalorimeterHits", "SimCalorimeterHits", "RawWavlenStructs", "MCParticles", "Leakages"], OutputLevel = DEBUG)

from Configurables import DigiSiPM
digi = DigiSiPM("DigiSiPM",
  # Hamamatsu S13615-1025
  signalLength = 500.,
  SiPMsize = 1.,
  DCR = 100e3,
  Xtalk = 0.03,
  sampling = 0.1,
  recovery = 20.,
  cellpitch = 25.,
  afterpulse = 0.03,
  falltimeFast = 50.,
  risetime = 1.,
  SNR = 30.,
  gateLength = 240.,
  OutputLevel = DEBUG
)

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
