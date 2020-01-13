#include "RootInterface.h"

RootInterface::RootInterface(const std::string& filename)
: fFilename(filename), fNumEvt(0) {
  init();
}

RootInterface::~RootInterface() {}

void RootInterface::init() {
  fEventData = new T();
  fFile = TFile::Open(fFilename.c_str(),"UPDATE");
}

void RootInterface::create(const std::string& name, const std::string& title) {
  fTree = new TTree(name.c_str(),name.c_str());
  fTree->Branch(title.c_str(),fEventData);
}

void RootInterface::set(const std::string& name, const std::string& title) {
  fTree = (TTree*)fFile->Get(name.c_str());
  fTree->SetBranchAddress(title.c_str(),&fEventData);
}

void RootInterface::fill(const T* evt) {
  *fEventData = *evt;
  fTree->Fill();
}

void RootInterface::read(T& evt) {
  fTree->GetEntry(fNumEvt);
  evt = *fEventData;
  fNumEvt++;
}

void RootInterface::write() {
  fFile->WriteTObject(fTree);
}

void RootInterface::close() {
  fFile->Close();
  if (fEventData) delete fEventData;
}
