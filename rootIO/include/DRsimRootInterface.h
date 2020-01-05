#ifndef DRsimRootInterface_h
#define DRsimRootInterface_h 1

#include "DRsimInterface.h"
#include "TFile.h"
#include "TTree.h"

class DRsimRootInterface {
public:
  DRsimRootInterface(const std::string& filename, const std::string& treename);
  ~DRsimRootInterface();

  void write(const DRsimInterface::DRsimEventData* evt);
  void close();

private:
  void init();

  TFile* fFile;
  TTree* fTree;
  std::string fFilename;
  std::string fTreename;
  DRsimInterface::DRsimEventData* fEventData;
};

#endif
