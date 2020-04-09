# dual-readout
Repository for GEANT4 simulation &amp; analysis of the dual-readout calorimeter.

## How-to
### Compile
After fetching the repository, do

    source setenv-cc7-gcc8.sh
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

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HEPMC_DIR/lib64:$FASTJET_DIR/lib:$PYTHIA_DIR/lib:$PWD/lib

### Running Pythia8
In build/Gen,

    ./P8ptcgun ptcgun.cmnd <seed> <filename>
    
generates `<filename>_<seed>.root`.

### Running GEANT4
#### 1. GEANT4 standalone particle gun
In build/DRsim,

    ./DRsim <run_macro> <filenumber> <filename>
    
generates, `<filename>_<filenumber>.root`

#### 2. Using HepMC input
This requires the ROOT file generated from `Gen`. Assuming the name of the file `<filename>_<filenumber>.root`,

    ./DRsim run_hepmc.mac <filenumber> <filename>
    
### Reconstruction
This requires the ROOT file generated from `DRsim`. Assuming the name of the file `<filename>_<filenumber>.root`, in build/Reco,

    ./Reco <filenumber> <filename>
    
### Analysis
This requires the ROOT file generated from `Reco`. Assuming the name of the file `<filename>_<filenumber>.root`, in build/analysis,

    ./<your_analysis_program> <filenumber> <filename>
    
### Precaution
Since GEANT4 takes very large amount of time per an event, P8ptcgun, DRsim and Reco are assumed to run a few events only per ROOT file. The executables can be run on parallel using `torque` or `condor`, and can be merged before analysis step using `hadd` from ROOT.
