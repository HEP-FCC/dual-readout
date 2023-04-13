# dual-readout
Repository for GEANT4 simulation &amp; analysis of the dual-readout calorimeter.

## How-to
### Compile
After fetching the repository, do

```sh
source init_hsf.sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<path_to_install_directory> ..
make -j4
make install
```

Note that to use the installed binary & library files, need to do following (assuming `$PWD=<path_to_install_directory>`)

```sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib64
export PYTHONPATH=$PYTHONPATH:$PWD/python
```

### Generating events
Generating events relies on [k4Gen](https://github.com/HEP-FCC/k4Gen), generates primary particle(s) in `HepMC` format with either particle gun or `Pythia8` then converts it to `edm4hep`. Please refer to [k4Gen/options](https://github.com/HEP-FCC/k4Gen/tree/b7c735e401298a8c72915819dc0404a83f46a0fe/k4Gen/options) for example configurations.

### Running GEANT4 simulation
The typical GEANT4 configuration for DRC is located at `DRsim/DRsimG4Components/test/runDRsim.py`. After modifying the configuration based on your needs, run

```sh
k4run runDRsim.py
```

Note that there are several `Gaudi` components specific to DRC - `SimG4OpticalPhysicsList`, `SimG4FastSimOpFiberRegion`, and `SimG4DRcaloActions`.

```python3
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
```

Optical physics is NOT simulated by `GEANT4` default physics list due to the extensive computing. `SimG4OpticalPhysicsList` configures the Cherenkov and scintillation process and let `GEANT4` track optical photons.

However, full tracking of optical photons makes the simulation extremely heavy to an unpractical scale (costs > 4-6 hours to simulate a 10 GeV e- event). It can be significantly improved (2-3 mins per 10 GeV e- event) by skipping exhaustive tracking of optical photons with a good approximation. `FastSimModelOpFiber` and `SimG4FastSimOpFiberRegion` define the fast simulation model and the corresponding region for tracking optical photons. Details of the logic can be found at [GEANT4 R&D meeting](https://indico.cern.ch/event/915715/#2-fast-optical-photon-transpor).

`SimG4DRcaloActions` is responsible for initializing `SimG4DRcaloSteppingAction`, which retrieves MC truth energy deposit inside non-active absorbers. The resulting MC-truth energy deposit and counted number of photoelectrons are stored in the `edm4hep` collection named "SimCalorimeterHits" and "RawCalorimeterHits". The timing structure of arrived optical photons is stored in the user-class `edm4hep::SparseVector` "RawTimeStructs".

### Digitization
SiPM digitization is based on the external package [SimSiPM](https://github.com/EdoPro98/SimSiPM), please refer to the repository for the details. The default `Gaudi` configuration template can be found on `DRdigi/test/runDigi.py`. After modifying the configuration based on your needs, run

```sh
k4run runDigi.py
```

Parameters for SiPM digitization can be defined in the configuration, for example,

```python3
from Configurables import DigiSiPM
digi = DigiSiPM("DigiSiPM",
  # Hamamatsu S13615-1025
  signalLength = 500., # ns
  SiPMsize = 1., # mm
  DCR = 100e3, # kHz
  Xtalk = 0.03, # probability
  sampling = 0.1, # ns
  recovery = 20., # ns
  cellpitch = 25., # um
  afterpulse = 0.03, # probability
  falltimeFast = 50., # ns
  risetime = 1., # ns
  SNR = 30., # dB
  gateLength = 240., # ns
  OutputLevel = DEBUG
)
```

### Calibration &amp; Reconstruction
The `Gaudi` component `DRcalib2D` calculates reconstructed energy from ADC counts based on the calibration constants defined at `DRreco/calib.csv`. This requires the ROOT file generated from `runDigi.py`. The default `Gaudi` configuration template can be found on `DRreco/test/runDRcalib.py`. After modifying the configuration based on your needs, run

```sh
k4run runDRcalib.py
```

### Analysis
This requires the ROOT file generated from `runDRcalib.py`. Assuming the name of the file `<filename.root>`,

    ./bin/analysis <filename.root> <histogram lower edge in [GeV]> <histogram higher edge in [GeV]>

Note that the `analysis` is a mere example based on only `ROOT`, `podio` and `edm4hep`. Please make sure that you have implemented desired plots based on your needs and physics process.

### Precaution
Since the `GEANT4` simulation takes a very large amount of time per an event, it is assumed to run a few dozens of events per run. It can be run on parallel using `torque` or `condor`, and can be merged before analysis step using `hadd` from `ROOT`.
