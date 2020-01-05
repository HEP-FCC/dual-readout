#include "DRsimRootInterface.h"

DRsimRootInterface::DRsimRootInterface(const std::string& filename, const std::string& treename)
: fFilename(filename), fTreename(treename) {
  init();
}

DRsimRootInterface::~DRsimRootInterface() {}

void DRsimRootInterface::init() {
  fEventData = new DRsimInterface::DRsimEventData();

  fFile = TFile::Open(fFilename.c_str(),"UPDATE");
  fTree = new TTree(fTreename.c_str(),fTreename.c_str());
  fTree->Branch("DRsimEventData",fEventData);
}

void DRsimRootInterface::write(const DRsimInterface::DRsimEventData* evt) {
  *fEventData = *evt;

  fTree->Fill();
}

void DRsimRootInterface::close() {
  fFile->WriteTObject(fTree);
  fFile->Close();

  if (fEventData) delete fEventData;
}
