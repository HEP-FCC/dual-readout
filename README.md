# dual-readout
Repository for GEANT4 simulation &amp; analysis of the dual-readout calorimeter.

## How-to
### Compile
After fetching the repository, do

    source init_lcg.sh
    source init_k4.sh
    source init_sipm.sh
    mkdir build
    cd build
    cmake ..
    make -j4

### Install
For a case that needs to install the package (e.g. `condor` requires file transfer), one can install the package via

    cmake -DCMAKE_INSTALL_PREFIX=<path_to_install_directory> ..
    make -j4
    make install

Note that to use the installed binary & library files, need to do following (assuming `$PWD=<path_to_install_directory>`)

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib64
    export PYTHONPATH=$PYTHONPATH:$PWD/python

### Running Pythia8
In the install directory,

    ./bin/P8ptcgun ptcgun.cmnd <seed> <filename>

generates `<filename>_<seed>.root`.

### Running GEANT4
#### 1. GEANT4 standalone particle gun
In the install directory,

    ./bin/DRsim <etc/run_macro> <filenumber> <filename>

generates, `<filename>_<filenumber>.root`

#### 2. Using HepMC input
This requires the ROOT file generated from `Gen`. Assuming the name of the file `<filename>_<filenumber>.root`,

    ./bin/DRsim etc/run_hepmc.mac <filenumber> <filename>

### Digitization
This requires the ROOT file generated from `DRsim`. The default `Gaudi` configuration template can be found on `test/runDigi.py`. After modifying the configuration based on your needs, run

    k4run `runDigi.py`

### Reconstruction
This requires the ROOT file generated from `runDigi.py`. The default `Gaudi` configuration template can be found on `test/runDRcalib.py`. After modifying the configuration based on your needs, run

    k4run `runDRcalib.py`

### Analysis
This requires the ROOT file generated from `runDRcalib.py`. Assuming the name of the file `<filename.root>`,

    ./bin/analysis <filename.root> <histogram lower edge in [GeV]> <histogram higher edge in [GeV]>

### Precaution
Since GEANT4 takes very large amount of time per an event, P8ptcgun, DRsim and Reco are assumed to run a few events only per ROOT file. The executables can be run on parallel using `torque` or `condor`, and can be merged before analysis step using `hadd` from ROOT.
