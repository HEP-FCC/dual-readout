#ifndef fastjetInterface_h
#define fastjetInterface_h 1

#include "fastjet/PseudoJet.hh"
#include "TTree.h"

class fastjetInterface {
public:
  struct fastjetDataBase {
    fastjetDataBase() {};
    fastjetDataBase(fastjet::PseudoJet& jet);
    virtual ~fastjetDataBase() {};

    double E;
    double px;
    double py;
    double pz;
  };

  struct fastjetData {
    fastjetData() {};
    fastjetData(fastjet::PseudoJet& jet);
    virtual ~fastjetData() {};

    double E;
    double px;
    double py;
    double pz;
    double phi;
    double phi_std;
    double rap;
    double eta;
    double pt;
    double m;
    double mt;
    bool hasAssociatedCS;
    bool validCS;
    bool hasConstituents;
    int nConstituents;
    bool hasChild;
    fastjetDataBase child;
    // bool hasExclusiveSubjets; #FIXME add exclusive jets if needed (with appropriate d_cut)
    // int nExclusiveSubjets;
  };

  fastjetInterface();
  ~fastjetInterface();

  void init(TTree* treeIn, std::string branchname);
  void writeJets(std::vector<fastjet::PseudoJet> jets);
  void runFastjet(const std::vector<fastjet::PseudoJet>& input);
  void set(TTree* treeIn, std::string branchname);
  void read(std::vector<fastjetData>& jets);

private:
  std::vector<fastjetData>* fJets;
  std::vector<fastjetDataBase>* fJetBase;

};

#endif
