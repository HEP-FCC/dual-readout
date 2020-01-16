#include "RootInterface.h"
#include "RecoInterface.h"
#include "DRsimInterface.h"

#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TString.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  TString filename = argv[1];
  float low = std::stof(argv[2]);
  float high = std::stof(argv[3]);

  TH1F* tEdep = new TH1F("totEdep","Total Energy deposit;MeV;Evt",100,low*1000.,high*1000.);
  tEdep->Sumw2(); tEdep->SetLineColor(kRed); tEdep->SetLineWidth(2);
  TH1F* tE_C = new TH1F("E_C","Energy of Cerenkov ch.;GeV;Evt",100,low,high);
  tE_C->Sumw2(); tE_C->SetLineColor(kBlue); tE_C->SetLineWidth(2);
  TH1F* tE_S = new TH1F("E_S","Energy of Scintillation ch.;GeV;Evt",100,low,high);
  tE_S->Sumw2(); tE_S->SetLineColor(kRed); tE_S->SetLineWidth(2);
  TH1F* tE_SC = new TH1F("E_SC","E_{S}+E_{C};GeV;Evt",100,2.*low,2.*high);
  tE_SC->Sumw2(); tE_SC->SetLineColor(kBlack); tE_SC->SetLineWidth(2);
  TH1F* tE_DR = new TH1F("E_DR","Dual-readout corrected Energy;GeV;Evt",100,low,high);
  tE_DR->Sumw2(); tE_DR->SetLineColor(kBlack); tE_DR->SetLineWidth(2);

  RootInterface<RecoInterface::RecoEventData>* recoInterface = new RootInterface<RecoInterface::RecoEventData>(std::string(filename)+".root");
  recoInterface->set("Reco","RecoEventData");

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(std::string(filename)+".root");
  drInterface->set("DRsim","DRsimEventData");

  unsigned int entries = recoInterface->entries();
  while (recoInterface->numEvt() < entries) {
    RecoInterface::RecoEventData evt;
    recoInterface->read(evt);

    DRsimInterface::DRsimEventData drEvt;
    drInterface->read(drEvt);

    float Edep = 0.;
    for (auto edepItr = drEvt.Edeps.begin(); edepItr != drEvt.Edeps.end(); ++edepItr) {
      auto edep = *edepItr;
      Edep += edep.Edep;
    }
    tEdep->Fill(Edep);

    tE_C->Fill(evt.E_C);
    tE_S->Fill(evt.E_S);
    tE_SC->Fill(evt.E_C+evt.E_S);
    tE_DR->Fill(evt.E_DR);
  } // event loop

  TCanvas* c = new TCanvas("c","");

  tEdep->Draw("Hist"); c->SaveAs(filename+"_Edep.png");

  TF1* grE_C = new TF1("Cfit","gaus",low,high); grE_C->SetLineColor(kBlue);
  TF1* grE_S = new TF1("Sfit","gaus",low,high); grE_S->SetLineColor(kRed);
  tE_C->SetOption("p"); tE_C->Fit(grE_C,"R+&same");
  tE_S->SetOption("p"); tE_S->Fit(grE_S,"R+&same");

  c->cd();
  tE_S->Draw(""); c->Update();
  TPaveStats* statsE_S = (TPaveStats*)c->GetPrimitive("stats");
  statsE_S->SetName("Scint");
  statsE_S->SetTextColor(kRed);
  statsE_S->SetY1NDC(.6); statsE_S->SetY2NDC(.8);

  tE_C->Draw("sames"); c->Update();
  TPaveStats* statsE_C = (TPaveStats*)c->GetPrimitive("stats");
  statsE_C->SetName("Cerenkov");
  statsE_C->SetTextColor(kBlue);
  statsE_C->SetY1NDC(.8); statsE_C->SetY2NDC(1.);

  c->SaveAs(filename+"_Ecs.png");
  tE_S->Draw("Hist");
  tE_C->Draw("Hist&sames");
  c->SaveAs(filename+"_EcsHist.png");

  TF1* grE_SC = new TF1("S+Cfit","gaus",2.*low,2.*high); grE_SC->SetLineColor(kBlack);
  TF1* grE_DR = new TF1("Efit","gaus",low,high); grE_DR->SetLineColor(kBlack);
  tE_SC->SetOption("p"); tE_SC->Fit(grE_SC,"R+&same");
  tE_DR->SetOption("p"); tE_DR->Fit(grE_DR,"R+&same");

  tE_SC->Draw(""); c->SaveAs(filename+"_Esum.png");
  tE_DR->Draw(""); c->SaveAs(filename+"_Ecorr.png");
}
