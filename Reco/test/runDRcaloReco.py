from Gaudi.Configuration import *
from Configurables import ApplicationMgr

from Configurables import k4DataSvc
dataservice = k4DataSvc("EventDataSvc", input="Ele20GeV_1_t0.root")

from Configurables import PodioInput
podioinput = PodioInput("PodioInput", collections = ["DRSimCalorimeterHits", "RawCalorimeterHits"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc(
    "GeoSvc",
    detectors = [
        'file:bin/compact/DRcalo.xml'
    ]
)

from Configurables import RecoFiber
reco = RecoFiber("RecoFiber", OutputLevel=DEBUG)

from Configurables import PodioOutput
out = PodioOutput("PodioOutput", filename = "Ele20GeV_1_reco.root", OutputLevel = DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [
        podioinput,
        reco,
        out
    ],
    EvtSel = 'NONE',
    EvtMax = 10,
    ExtSvc = [dataservice, geoservice]
)
