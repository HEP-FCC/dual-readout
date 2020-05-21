#include "RootInterface.h"
#include "RecoInterface.h"
#include "DRsimInterface.h"
#include "functions.h"

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

int main(int argc, char* argv[]) {
  TString filename = argv[1];
  float low = std::stof(argv[2]);
  float high = std::stof(argv[3]);

  gStyle->SetOptFit(1);

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
  TH1F* tP_leak = new TH1F("Pleak","Momentum leak;MeV;Evt",100,0.,1000.*high);
  tP_leak->Sumw2(); tP_leak->SetLineWidth(2);
  TH1F* tP_leak_nu = new TH1F("Pleak_nu","Neutrino energy leak;MeV;Evt",100,0.,1000.*high);
  tP_leak_nu->Sumw2(); tP_leak_nu->SetLineWidth(2);
  TH1F* tDepth = new TH1F("depth","Incident particle depth;m;Evt",100,-0.5,2.5);
  tDepth->Sumw2(); tDepth->SetLineWidth(2);

  TH1F* tEdep_noLeak = new TH1F("totEdep_noLeak","Total Energy deposit;MeV;Evt",100,low*1000.,high*1000.);
  tEdep_noLeak->Sumw2(); tEdep_noLeak->SetLineColor(kRed); tEdep_noLeak->SetLineWidth(2);

  TH1F* tT_C = new TH1F("time_C","Cerenkov time;ns;p.e.",150,10.,70.);
  tT_C->Sumw2(); tT_C->SetLineColor(kBlue); tT_C->SetLineWidth(2);
  TH1F* tT_S = new TH1F("time_S","Scint time;ns;p.e.",150,10.,70.);
  tT_S->Sumw2(); tT_S->SetLineColor(kRed); tT_S->SetLineWidth(2);
  TH1F* tWav_S = new TH1F("wavlen_S","Scint wavelength;nm;p.e.",120,300.,900.);
  tWav_S->Sumw2(); tWav_S->SetLineColor(kRed); tWav_S->SetLineWidth(2);
  TH1F* tWav_C = new TH1F("wavlen_C","Cerenkov wavelength;nm;p.e.",120,300.,900.);
  tWav_C->Sumw2(); tWav_C->SetLineColor(kBlue); tWav_C->SetLineWidth(2);
  TH1F* tNhit_S = new TH1F("nHits_S","Number of Scint p.e./SiPM;p.e.;n",200,0.,200.);
  tNhit_S->Sumw2(); tNhit_S->SetLineColor(kRed); tNhit_S->SetLineWidth(2);
  TH1F* tNhit_C = new TH1F("nHits_C","Number of Cerenkov p.e./SiPM;p.e.;n",50,0.,50.);
  tNhit_C->Sumw2(); tNhit_C->SetLineColor(kBlue); tNhit_C->SetLineWidth(2);

  RootInterface<RecoInterface::RecoEventData>* recoInterface = new RootInterface<RecoInterface::RecoEventData>(std::string(filename)+".root");
  recoInterface->set("Reco","RecoEventData");

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(std::string(filename)+".root");
  drInterface->set("DRsim","DRsimEventData");

  int nLeak = 0;
  std::vector<float> E_Ss,E_Cs;
  std::vector<float> E_Ss_noLeak,E_Cs_noLeak;

  unsigned int entries = recoInterface->entries();
  while (recoInterface->numEvt() < entries) {
    if (recoInterface->numEvt() % 100 == 0) printf("Analyzing %dth event ...\n", recoInterface->numEvt());

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

    float Pleak = 0.;
    float Eleak_nu = 0.;
    for (auto leak : drEvt.leaks) {
      TLorentzVector leak4vec;
      leak4vec.SetPxPyPzE(leak.px,leak.py,leak.pz,leak.E);
      if ( std::abs(leak.pdgId)==12 || std::abs(leak.pdgId)==14 || std::abs(leak.pdgId)==16 ) {
        Eleak_nu += leak4vec.P();
      } else {
        Pleak += leak4vec.P();
      }
    }
    tP_leak->Fill(Pleak);
    tP_leak_nu->Fill(Eleak_nu);

    TH1F* tT_max = new TH1F("tmax","",600,10.,70.);

    for (auto tower = drEvt.towers.begin(); tower != drEvt.towers.end(); ++tower) {
      for (auto sipm = tower->SiPMs.begin(); sipm != tower->SiPMs.end(); ++sipm) {
        if ( RecoInterface::IsCerenkov(sipm->x,sipm->y) ) {
          tNhit_C->Fill(sipm->count);

          for (const auto timepair : sipm->timeStruct) {
            tT_C->Fill(timepair.first.first+0.05,timepair.second);
            tT_max->Fill(timepair.first.first+0.05,timepair.second);
          }
          for (const auto wavpair : sipm->wavlenSpectrum) {
            tWav_C->Fill(wavpair.first.first,wavpair.second);
          }
        } else {
          tNhit_S->Fill(sipm->count);

          for (const auto timepair : sipm->timeStruct) {
            tT_S->Fill(timepair.first.first+0.05,timepair.second);
          }
          for (const auto wavpair : sipm->wavlenSpectrum) {
            tWav_S->Fill(wavpair.first.first,wavpair.second);
          }
        }
      }
    }

    float T_max = tT_max->GetBinCenter( tT_max->GetMaximumBin() );
    float depth = ( T_max - 1.8/0.3 - 2.0/0.1895 ) / ( 1./0.3 - 1./0.1895 ); // 1895

    delete tT_max;

    float E_Scorr = evt.E_S*std::exp( -(depth-0.1368)/12.78 );

    E_Ss.push_back(E_Scorr);
    E_Cs.push_back(evt.E_C);

    if (Pleak > 3000.) {
      nLeak++;
      continue;
    }

    tEdep_noLeak->Fill(Edep);

    E_Ss_noLeak.push_back(E_Scorr);
    E_Cs_noLeak.push_back(evt.E_C);

    tE_C->Fill(evt.E_C);
    tE_S->Fill(evt.E_S);
    tE_SC->Fill(evt.E_C+E_Scorr);
    // tE_SC->Fill(evt.E_C+evt.E_S);
    // tE_DR->Fill(evt.E_DR);
    tE_DR->Fill(functions::E_DR291(evt.E_C,E_Scorr));
    tDepth->Fill(depth);
  } // event loop

  TCanvas* c = new TCanvas("c","");

  tEdep->Draw("Hist"); c->SaveAs(filename+"_Edep.png");
  tEdep_noLeak->Draw("Hist"); c->SaveAs(filename+"_Edep_noLeak.png");

  c->cd();
  tE_S->SetTitle("");
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

  TF1* grE_C = new TF1("Cfit","gaus",low,high); grE_C->SetLineColor(kBlue);
  TF1* grE_S = new TF1("Sfit","gaus",low,high); grE_S->SetLineColor(kRed);
  tE_C->SetOption("p"); tE_C->Fit(grE_C,"R+&same");
  tE_S->SetOption("p"); tE_S->Fit(grE_S,"R+&same");

  c->cd();
  tE_S->SetTitle("");
  tE_S->Draw(""); c->Update();
  statsE_S->SetName("Scint");
  statsE_S->SetTextColor(kRed);
  statsE_S->SetX1NDC(.7);
  statsE_S->SetY1NDC(.4); statsE_S->SetY2NDC(.7);

  tE_C->Draw("sames"); c->Update();
  statsE_C->SetName("Cerenkov");
  statsE_C->SetTextColor(kBlue);
  statsE_C->SetX1NDC(.7);
  statsE_C->SetY1NDC(.7); statsE_C->SetY2NDC(1.);

  c->SaveAs(filename+"_Ecs.png");

  TF1* grE_SC = new TF1("S+Cfit","gaus",2.*low,2.*high); grE_SC->SetLineColor(kBlack);
  TF1* grE_DR = new TF1("Efit","gaus",low,high); grE_DR->SetLineColor(kBlack);
  tE_SC->SetOption("p"); tE_SC->Fit(grE_SC,"R+&same");
  tE_DR->SetOption("p"); tE_DR->Fit(grE_DR,"R+&same");

  tE_SC->Draw(""); c->SaveAs(filename+"_Esum.png");
  tE_DR->Draw(""); c->SaveAs(filename+"_Ecorr.png");

  tDepth->Draw("Hist"); c->SaveAs(filename+"_depth.png");

  c->SetLogy(1);
  tP_leak->Draw("Hist"); c->SaveAs(filename+"_Pleak.png");
  tP_leak_nu->Draw("Hist"); c->SaveAs(filename+"_Pleak_nu.png");
  c->SetLogy(0);

  TGraph* grSvsC = new TGraph(entries,&(E_Ss[0]),&(E_Cs[0]));
  grSvsC->SetTitle("SvsC;E_S;E_C");
  grSvsC->SetMarkerSize(0.5); grSvsC->SetMarkerStyle(20);
  grSvsC->GetXaxis()->SetLimits(0.,high);
  grSvsC->GetYaxis()->SetRangeUser(0.,high);
  grSvsC->SetMaximum(high);
  grSvsC->SetMinimum(0.);
  grSvsC->Draw("ap");
  c->SaveAs(filename+"_SvsC.png");

  TGraph* grSvsC_noLeak = new TGraph(entries-nLeak,&(E_Ss_noLeak[0]),&(E_Cs_noLeak[0]));
  grSvsC_noLeak->SetTitle("SvsC_noLeak;E_S;E_C");
  grSvsC_noLeak->SetMarkerSize(0.5); grSvsC_noLeak->SetMarkerStyle(20);
  grSvsC_noLeak->GetXaxis()->SetLimits(0.,high);
  grSvsC_noLeak->GetYaxis()->SetRangeUser(0.,high);
  grSvsC_noLeak->SetMaximum(high);
  grSvsC_noLeak->SetMinimum(0.);
  grSvsC_noLeak->Draw("ap");
  c->SaveAs(filename+"_SvsC_noLeak.png");

  tT_C->Draw("Hist"); c->SaveAs(filename+"_tC.png");
  tT_S->Draw("Hist"); c->SaveAs(filename+"_tS.png");
  tWav_C->Draw("Hist"); c->SaveAs(filename+"_wavC.png");
  tWav_S->Draw("Hist"); c->SaveAs(filename+"_wavS.png");
  tNhit_C->Draw("Hist"); c->SaveAs(filename+"_nhitC.png");
  tNhit_S->Draw("Hist"); c->SaveAs(filename+"_nhitS.png");
}
