#include "RootInterface.h"
#include "DRsimInterface.h"
#include "RecoInterface.h"

template <typename T>
RootInterface<T>::RootInterface(const std::string& filename)
: fFilename(filename), fNumEvt(0) {
  init();
}

template <typename T>
RootInterface<T>::~RootInterface() {}

template <typename T>
void RootInterface<T>::init() {
  fEventData = new T();
  fFile = TFile::Open(fFilename.c_str(),"UPDATE");
}

template <typename T>
void RootInterface<T>::create(const std::string& name, const std::string& title) {
  fTree = new TTree(name.c_str(),name.c_str());
  fTree->Branch(title.c_str(),fEventData);
}

template <typename T>
void RootInterface<T>::set(const std::string& name, const std::string& title) {
  fTree = (TTree*)fFile->Get(name.c_str());
  fTree->SetBranchAddress(title.c_str(),&fEventData);
}

template <typename T>
void RootInterface<T>::fill(const T* evt) {
  *fEventData = *evt;
  fTree->Fill();
}

template <typename T>
void RootInterface<T>::read(T& evt) {
  fTree->GetEntry(fNumEvt);
  evt = *fEventData;
  fNumEvt++;
}

template <typename T>
void RootInterface<T>::write() {
  fFile->WriteTObject(fTree);
}

template <typename T>
void RootInterface<T>::close() {
  fFile->Close();
  if (fEventData) delete fEventData;
}

template <typename T>
TTree* RootInterface<T>::getTree() {
  return fTree;
}

template class RootInterface<DRsimInterface::DRsimEventData>;
template class RootInterface<RecoInterface::RecoEventData>;
