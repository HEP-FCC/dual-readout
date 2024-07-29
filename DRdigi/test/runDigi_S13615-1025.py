from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import k4DataSvc

dataservice = k4DataSvc("EventDataSvc", input="sim.root")

from Configurables import PodioInput

podioinput = PodioInput(
    "PodioInput",
    collections=[
        "RawTimeStructs",
        "RawCalorimeterHits",
        "SimCalorimeterHits",
        "RawWavlenStructs",
        "GenParticles",
        "Leakages",
    ],
    OutputLevel=DEBUG,
)

from Configurables import DigiSiPM

digi = DigiSiPM(
    "DigiSiPM",
    # Hamamatsu S13615-1025
    signalLength=500.0,
    SiPMsize=1.0,
    DCR=100e3,
    Xtalk=0.03,
    sampling=0.1,
    recovery=20.0,
    cellpitch=25.0,
    afterpulse=0.03,
    falltimeFast=50.0,
    risetime=1.0,
    SNR=30.0,
    gateLength=240.0,
    OutputLevel=DEBUG,
)

from Configurables import PodioOutput

podiooutput = PodioOutput("PodioOutput", filename="digi.root", OutputLevel=DEBUG)
podiooutput.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg=[podioinput, digi, podiooutput],
    EvtSel="NONE",
    EvtMax=10,
    ExtSvc=[dataservice],
)
