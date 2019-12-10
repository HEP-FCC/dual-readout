#include "fastjet/PseudoJet.hh"

class fastjetDataBase {
  double E;
  double px;
  double py;
  double pz;
};

class fastjetData {
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
  // bool hasExclusiveSubjets; #FIXME add exclusive jets if needed (with appropriate d_cut)
  // int nExclusiveSubjets;
  bool hasArea;
  double area;
  double areaErr;
  bool isPureGhost;
};
