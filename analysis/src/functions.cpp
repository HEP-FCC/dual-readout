#include "functions.h"
#include "TLorentzVector.h"

float functions::E_DR(float E_C, float E_S) {
  float hOe_C = 0.2484;
  float hOe_S = 0.8342;
  float chi = (1. - hOe_S) / (1. - hOe_C);

  return (E_S - chi * E_C) / (1 - chi);
}

float functions::E_DR291(float E_C, float E_S) {
  float chi = 0.291;

  return (E_S - chi * E_C) / (1 - chi);
}

functions::dualhist1D::dualhist1D(TString name, TString title, int nbins, double start, double end) {
  mHistS = std::make_unique<TH1F>(name + "_S", "Scint " + title, nbins, start, end);
  mHistS->Sumw2();
  mHistS->SetLineColor(kRed);
  mHistS->SetLineWidth(2);
  mHistC = std::make_unique<TH1F>(name + "_C", "Cherenkov " + title, nbins, start, end);
  mHistC->Sumw2();
  mHistC->SetLineColor(kBlue);
  mHistC->SetLineWidth(2);
}
