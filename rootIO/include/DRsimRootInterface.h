#ifndef DRsimRootInterface_h
#define DRsimRootInterface_h 1

#include "DRsimInterface.h"
#include "TFile.h"
#include "TTree.h"

class DRsimRootInterface {
public:
  DRsimRootInterface(const std::string& filename);
  ~DRsimRootInterface();

  void fill(const DRsimInterface::DRsimEventData* evt);
  void read(DRsimInterface::DRsimEventData& evt);
  void create();
  void set();
  void write();
  void close();

  unsigned int entries() { return fTree->GetEntries(); }
  unsigned int numEvt() { return fNumEvt; }

private:
  void init();

  TFile* fFile;
  TTree* fTree;
  std::string fFilename;
  DRsimInterface::DRsimEventData* fEventData;
  unsigned int fNumEvt;
};

#endif
