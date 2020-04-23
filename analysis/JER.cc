#include "RootInterface.h"
#include "RecoInterface.h"
#include "DRsimInterface.h"
#include "fastjetInterface.h"
#include "functions.h"

#include "fastjet/PseudoJet.hh"

#include "HepMC3/ReaderRootTree.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TGraph.h"

#include <iostream>
#include <string>
#include <cmath>

int main(int argc, char* argv[]) {
  TString filename = argv[1];
  float low = std::stof(argv[2]);
  float high = std::stof(argv[3]);

  gStyle->SetOptFit(1);

  TH1F* tEdep = new TH1F("totEdep","Total Energy deposit;MeV;Evt",100,2.*low*1000.,2.*high*1000.);
  tEdep->Sumw2(); tEdep->SetLineColor(kRed); tEdep->SetLineWidth(2);
  TH1F* tE_C = new TH1F("E_C","Energy of Cerenkov ch.;GeV;Evt",100,2.*low,2.*high);
  tE_C->Sumw2(); tE_C->SetLineColor(kBlue); tE_C->SetLineWidth(2);
  TH1F* tE_S = new TH1F("E_S","Energy of Scintillation ch.;GeV;Evt",100,2.*low,2.*high);
  tE_S->Sumw2(); tE_S->SetLineColor(kRed); tE_S->SetLineWidth(2);
  TH1F* tE_DR = new TH1F("E_DR","Dual-readout corrected Energy;GeV;Evt",100,2.*low,2.*high);
  tE_DR->Sumw2(); tE_DR->SetLineColor(kBlack); tE_DR->SetLineWidth(2);

  TH1F* tE_Sjets = new TH1F("E_Sjets","Energy of Scint cluster;GeV;nJets",100,low,high);
  tE_Sjets->Sumw2(); tE_Sjets->SetLineColor(kRed); tE_Sjets->SetLineWidth(2);
  TH1F* tE_Cjets = new TH1F("E_Cjets","Energy of Cerenkov cluster;GeV;nJets",100,low,high);
  tE_Cjets->Sumw2(); tE_Cjets->SetLineColor(kBlue); tE_Cjets->SetLineWidth(2);
  TH1F* tE_GenJets = new TH1F("E_GenJets","Energy of GenJets;GeV;nJets",100,low,high);
  tE_GenJets->Sumw2(); tE_GenJets->SetLineColor(kBlack); tE_GenJets->SetLineWidth(2);

  TH1F* tE_DRjets = new TH1F("E_DRjets","Energy of DR corrected cluster;GeV;nJets",100,low,high);
  tE_DRjets->Sumw2(); tE_DRjets->SetLineColor(kBlack); tE_DRjets->SetLineWidth(2);
  TH1F* tE_diffTruth = new TH1F("E_diffTruth","E_DR - Edep;GeV;Evt",100,-high/2.,high/2.);
  tE_diffTruth->Sumw2(); tE_diffTruth->SetLineColor(kBlack); tE_diffTruth->SetLineWidth(2);

  TH1F* tdR_jets = new TH1F("dR_jets","#Delta R between S & C jets;#Delta R;nJets",100,0.,5.);
  tdR_jets->Sumw2(); tdR_jets->SetLineWidth(2);
  TH1F* tE_tot = new TH1F("Etot","Total energy;GeV;Evt",100,2.*low,2.*high);
  tE_tot->Sumw2(); tE_tot->SetLineWidth(2);
  TH1F* tE_leak = new TH1F("Eleak","Energy leak;MeV;Evt",100,0.,1000.*high/2.);
  tE_leak->Sumw2(); tE_leak->SetLineWidth(2);

  RootInterface<RecoInterface::RecoEventData>* recoInterface = new RootInterface<RecoInterface::RecoEventData>(std::string(filename)+".root");
  recoInterface->set("Reco","RecoEventData");

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(std::string(filename)+".root");
  drInterface->set("DRsim","DRsimEventData");

  HepMC3::ReaderRootTree reader(std::string(filename)+".root");

  // fastjetInterface fjTower_S;
  // fjTower_S.set(recoInterface->getTree(),"RecoTowerJets_S");
  // fastjetInterface fjFiber_S;
  // fjFiber_S.set(recoInterface->getTree(),"RecoFiberJets_S");
  // fastjetInterface fjFiber_C;
  // fjFiber_C.set(recoInterface->getTree(),"RecoFiberJets_C");
  // fastjetInterface fjGen;
  // fjGen.set(reader.m_tree,"GenJets");

  DRsimInterface::DRsimTimeStruct tmp;
  std::vector<float> E_Ss,E_Cs;

  unsigned int entries = recoInterface->entries();
  while (recoInterface->numEvt() < entries) {
    if (recoInterface->numEvt() % 50 == 0) printf("Analyzing %dth event ...\n", recoInterface->numEvt());

    RecoInterface::RecoEventData evt;
    recoInterface->read(evt);

    DRsimInterface::DRsimEventData drEvt;
    drInterface->read(drEvt);

    HepMC3::GenEvent genEvt;
    reader.read_event(genEvt);

    std::vector<fastjet::PseudoJet> fjInputs_G;
    std::vector<fastjet::PseudoJet> fjInputs_C;
    std::vector<fastjet::PseudoJet> fjInputs_S;

    float Etot = 0.;
    for (auto ptc : genEvt.particles()) {
      int abspid = std::abs(ptc->pid());
      if ( ptc->status() != 1 ) continue;
      if ( abspid == 12 || abspid == 14 || abspid == 16 ) continue;

      auto mom = ptc->momentum();

      Etot += mom.e();
      fjInputs_G.push_back( fastjet::PseudoJet(mom.px(),mom.py(),mom.pz(),mom.e()) );
    }

    float Eleak = 0.;
    for (auto leak : drEvt.leaks) {
      Eleak += leak.E;
    }

    // std::vector<fastjetInterface::fastjetData> fjTS;
    // fjTower_S.read(fjTS);
    //
    // std::vector<fastjetInterface::fastjetData> fjFS;
    // fjFiber_S.read(fjFS);
    //
    // std::vector<fastjetInterface::fastjetData> fjFC;
    // fjFiber_C.read(fjFC);

    // std::vector<fastjetInterface::fastjetData> fjG;
    // fjGen.read(fjG);

    float Edep = 0.;
    for (auto edep : drEvt.Edeps) {
      Edep += edep.Edep;
    }

    for (auto tower : evt.towers) {
      for (auto fiber : tower.fibers) {
        TVector3 vec(std::get<0>(fiber.pos),std::get<1>(fiber.pos),std::get<2>(fiber.pos));
        TVector3 p = fiber.E*vec.Unit();

        if (fiber.IsCerenkov) {
          fjInputs_C.push_back( fastjet::PseudoJet(p.x(),p.y(),p.z(),fiber.E) );
        } else {
          fjInputs_S.push_back( fastjet::PseudoJet(p.x(),p.y(),p.z(),fiber.E) );
        }
      }
    }

    double dR = 0.8;
    auto fjG = functions::runFastjet(fjInputs_G,dR);
    auto fjFS = functions::runFastjet(fjInputs_S,dR);
    auto fjFC = functions::runFastjet(fjInputs_C,dR);

    tEdep->Fill(Edep);
    tE_tot->Fill(Etot);
    tE_leak->Fill(Eleak);

    tE_C->Fill(evt.E_C);
    tE_S->Fill(evt.E_S);
    tE_DR->Fill(evt.E_DR);
    tE_diffTruth->Fill(evt.E_DR-Edep/1000.);

    tE_Sjets->Fill(fjFS.at(0).E);
    tE_Cjets->Fill(fjFC.at(0).E);
    tE_GenJets->Fill(fjG.at(0).E);

    TLorentzVector firstS4vec, firstC4vec, firstG4vec;
    firstS4vec.SetPxPyPzE(fjFS.at(0).px,fjFS.at(0).py,fjFS.at(0).pz,fjFS.at(0).E);
    firstC4vec.SetPxPyPzE(fjFC.at(0).px,fjFC.at(0).py,fjFC.at(0).pz,fjFC.at(0).E);
    firstG4vec.SetPxPyPzE(fjG.at(0).px,fjG.at(0).py,fjG.at(0).pz,fjG.at(0).E);

    auto secondS = functions::findSecondary(fjFS,dR);
    auto secondC = functions::findSecondary(fjFC,dR);
    auto secondG = functions::findSecondary(fjG,dR);

    tE_Sjets->Fill(secondS.E);
    tE_Cjets->Fill(secondC.E);
    tE_GenJets->Fill(secondG.E);

    TLorentzVector secondS4vec, secondC4vec, secondG4vec;
    secondS4vec.SetPxPyPzE(secondS.px,secondS.py,secondS.pz,secondS.E);
    secondC4vec.SetPxPyPzE(secondC.px,secondC.py,secondC.pz,secondC.E);
    secondG4vec.SetPxPyPzE(secondG.px,secondG.py,secondG.pz,secondG.E);

    float E_DRjets1, E_DRjets2;
    if ( secondS4vec.DeltaR(secondC4vec) < 0.1 && firstS4vec.DeltaR(firstC4vec) < 0.1 ) {
      E_DRjets1 = functions::E_DR(fjFC.at(0).E,fjFS.at(0).E);
      E_DRjets2 = functions::E_DR(secondC.E,secondS.E);

      tE_DRjets->Fill(E_DRjets1); E_Ss.push_back(fjFS.at(0).E); E_Cs.push_back(fjFC.at(0).E);
      tE_DRjets->Fill(E_DRjets2); E_Ss.push_back(secondS.E); E_Cs.push_back(secondC.E);
    } else if ( secondS4vec.DeltaR(firstC4vec) < 0.1 && firstS4vec.DeltaR(secondC4vec) < 0.1 ) {
      E_DRjets2 = functions::E_DR(fjFC.at(0).E,secondS.E);
      E_DRjets1 = functions::E_DR(secondC.E,fjFS.at(0).E);

      tE_DRjets->Fill(E_DRjets1); E_Ss.push_back(fjFS.at(0).E); E_Cs.push_back(fjFC.at(0).E);
      tE_DRjets->Fill(E_DRjets2); E_Ss.push_back(secondS.E); E_Cs.push_back(secondC.E);
    } else continue;
  } // event loop

  reader.close();
  drInterface->close();
  recoInterface->close();

  TCanvas* c = new TCanvas("c","");

  tEdep->Draw("Hist"); c->SaveAs(filename+"_Edep.png");
  tE_tot->Draw("Hist"); c->SaveAs(filename+"_Etot.png");
  tE_leak->Draw("Hist"); c->SaveAs(filename+"_Eleak.png");

  c->cd();
  tE_S->Draw("Hist"); c->Update();
  TPaveStats* statsE_S = (TPaveStats*)c->GetPrimitive("stats");
  statsE_S->SetName("Scint");
  statsE_S->SetTextColor(kRed);
  statsE_S->SetY1NDC(.6); statsE_S->SetY2NDC(.8);

  tE_C->Draw("Hist&sames"); c->Update();
  TPaveStats* statsE_C = (TPaveStats*)c->GetPrimitive("stats");
  statsE_C->SetName("Cerenkov");
  statsE_C->SetTextColor(kBlue);
  statsE_C->SetY1NDC(.8); statsE_C->SetY2NDC(1.);

  c->SaveAs(filename+"_EcsHist.png");

  TF1* grE_DR = new TF1("Efit","gaus",2.*low,2.*high); grE_DR->SetLineColor(kBlack);
  tE_DR->SetOption("p"); tE_DR->Fit(grE_DR,"R+&same");
  tE_DR->Draw(""); c->SaveAs(filename+"_Ecorr.png");

  TF1* grE_diffTruth = new TF1("EdiffTruthFit","gaus",-high/2.,high/2.); grE_diffTruth->SetLineColor(kBlack);
  tE_diffTruth->SetOption("p"); tE_diffTruth->Fit(grE_diffTruth,"R+&same");
  tE_diffTruth->Draw(""); c->SaveAs(filename+"_EdiffTruth.png");

  tE_Sjets->Draw(""); c->Update();
  TPaveStats* statsE_Sjets = (TPaveStats*)c->GetPrimitive("stats");
  statsE_Sjets->SetName("Scint");
  statsE_Sjets->SetTextColor(kRed);
  statsE_Sjets->SetY1NDC(.6); statsE_Sjets->SetY2NDC(.8);

  tE_Cjets->Draw("sames"); c->Update();
  TPaveStats* statsE_Cjets = (TPaveStats*)c->GetPrimitive("stats");
  statsE_Cjets->SetName("Cerenkov");
  statsE_Cjets->SetTextColor(kBlue);
  statsE_Cjets->SetY1NDC(.8); statsE_Cjets->SetY2NDC(1.);

  tE_Sjets->Draw("Hist");
  tE_Cjets->Draw("Hist&sames");
  c->SaveAs(filename+"_EcsJetsHist.png");

  tE_GenJets->Draw("Hist");
  c->SaveAs(filename+"_EGenjets.png");

  TF1* grE_DRjets = new TF1("EjetsFit","gaus",low,high); grE_DRjets->SetLineColor(kBlack);
  tE_DRjets->SetOption("p"); tE_DRjets->Fit(grE_DRjets,"R+&same");
  tE_DRjets->Draw("");
  c->SaveAs(filename+"_EDRjets.png");

  TGraph* grSvsC = new TGraph(entries,&(E_Ss[0]),&(E_Cs[0]));
  grSvsC->SetTitle("SvsC;E_S;E_C");
  grSvsC->SetMarkerSize(0.5); grSvsC->SetMarkerStyle(20);
  grSvsC->GetXaxis()->SetRangeUser(0.,high);
  grSvsC->GetYaxis()->SetRangeUser(0.,high);
  grSvsC->SetMaximum(high);
  grSvsC->SetMinimum(0.);
  grSvsC->Draw("ap");
}
