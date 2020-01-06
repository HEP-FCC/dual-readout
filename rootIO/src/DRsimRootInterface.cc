#include "DRsimRootInterface.h"

DRsimRootInterface::DRsimRootInterface(const std::string& filename)
: fFilename(filename), fNumEvt(0) {
  init();
}

DRsimRootInterface::~DRsimRootInterface() {}

void DRsimRootInterface::init() {
  fEventData = new DRsimInterface::DRsimEventData();
  fFile = TFile::Open(fFilename.c_str(),"UPDATE");
}

void DRsimRootInterface::create() {
  fTree = new TTree("DRsim","DRsim");
  fTree->Branch("DRsimEventData",fEventData);
}

void DRsimRootInterface::set() {
  fTree = (TTree*)fFile->Get("DRsim");
  fTree->SetBranchAddress("DRsimEventData",&fEventData);
}

void DRsimRootInterface::fill(const DRsimInterface::DRsimEventData* evt) {
  *fEventData = *evt;
  fTree->Fill();
}

void DRsimRootInterface::read(DRsimInterface::DRsimEventData& evt) {
  fTree->GetEntry(fNumEvt);
  evt = *fEventData;
  fNumEvt++;
}

void DRsimRootInterface::write() {
  fFile->WriteTObject(fTree);
}

void DRsimRootInterface::close() {
  fFile->Close();
  if (fEventData) delete fEventData;
}
