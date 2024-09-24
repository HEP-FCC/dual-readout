#include "functions.h"

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/TimeSeriesCollection.h"
#include "edm4hep/RawTimeSeriesCollection.h"

#include "podio/ROOTReader.h"
#include "podio/Frame.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TGraph.h"

#include "Math/Vector3Dfwd.h"
#include "Math/Vector3D.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  float low = std::stof(argv[1]);
  float high = std::stof(argv[2]);
  std::vector<std::string> filenames;

  for (int idx = 3; idx < argc; idx++)
    filenames.push_back( std::string(argv[idx]) );

  gStyle->SetOptFit(1);

  TH1F* tEdep = new TH1F("totEdep","Total Energy deposit;GeV;Evt",100,low,high);
  tEdep->Sumw2(); tEdep->SetLineColor(kRed); tEdep->SetLineWidth(2);
  TH1F* tE_C = new TH1F("E_C","Energy of Cerenkov ch.;GeV;Evt",80,low,high);
  tE_C->Sumw2(); tE_C->SetLineColor(kBlue); tE_C->SetLineWidth(2);
  TH1F* tE_S = new TH1F("E_S","Energy of Scintillation ch.;GeV;Evt",80,low,high);
  tE_S->Sumw2(); tE_S->SetLineColor(kRed); tE_S->SetLineWidth(2);
  TH1F* tE_SC = new TH1F("E_SC","E_{S}+E_{C};GeV;Evt",80,2.*low,2.*high);
  tE_SC->Sumw2(); tE_SC->SetLineColor(kBlack); tE_SC->SetLineWidth(2);
  TH1F* tE_DR = new TH1F("E_DR","Dual-readout corrected Energy;GeV;Evt",80,low,high);
  tE_DR->Sumw2(); tE_DR->SetLineColor(kBlack); tE_DR->SetLineWidth(2);

  functions::dualhist1D* tT_dual = new functions::dualhist1D("time","time;ns;p.e.",150,10.,70.);
  functions::dualhist1D* tWav_dual = new functions::dualhist1D("wavlen","wavelength;nm;p.e.",120,300.,900.);
  functions::dualhist1D* tNhit_dual = new functions::dualhist1D("nHits","of Scint p.e./SiPM;p.e.;n",100,0.,100.);
  functions::dualhist1D* tD_dual = new functions::dualhist1D("digi","digi waveform;ns;a.u.",300,10.,100.);
  functions::dualhist1D* tToa_dual = new functions::dualhist1D("ToA","Time of Arrival;ns;a.u.",150,10.,70.);
  functions::dualhist1D* tInt_dual = new functions::dualhist1D("Int","ADC integration;ADC counts;a.u.",500,0.,500.);
  functions::dualhist1D* tProc_dual = new functions::dualhist1D("proc","processed waveform;ns;a.u.",475,5.,100.);

  std::vector<float> E_Ss,E_Cs;

  auto reader = podio::ROOTReader();
  reader.openFiles(filenames);

  unsigned int entries = reader.getEntries("events");

  for (unsigned int iEvt = 0; iEvt < entries; iEvt++) {
    if (iEvt % 100 == 0) printf("Analyzing %dth event ...\n", iEvt);

    auto aframe = podio::Frame(reader.readEntry("events",iEvt));

    const auto* edepHits = static_cast<const edm4hep::SimCalorimeterHitCollection*>(aframe.get("SimCalorimeterHits"));
    const auto* rawTimeStructs = static_cast<const edm4hep::RawTimeSeriesCollection*>(aframe.get("RawTimeStructs"));
    const auto* rawWavlenStructs = static_cast<const edm4hep::RawTimeSeriesCollection*>(aframe.get("RawWavlenStructs"));
    const auto* digiWaveforms = static_cast<const edm4hep::TimeSeriesCollection*>(aframe.get("DigiWaveforms"));
    const auto* caloHits = static_cast<const edm4hep::CalorimeterHitCollection*>(aframe.get("DRcalo2dHits"));
    const auto* rawHits = static_cast<const edm4hep::RawCalorimeterHitCollection*>(aframe.get("RawCalorimeterHits"));
    const auto* digiHits = static_cast<const edm4hep::CalorimeterHitCollection*>(aframe.get("DigiCalorimeterHits"));
    const auto* procTimes = static_cast<const edm4hep::TimeSeriesCollection*>(aframe.get("DRpostprocTime"));
    const auto* leakages = static_cast<const edm4hep::MCParticleCollection*>(aframe.get("Leakages"));

    float leakage = 0.;

    for (unsigned int iLeak = 0; iLeak < leakages->size(); iLeak++) {
      auto& momentum = (*leakages)[iLeak].getMomentum();
      ROOT::Math::XYZVectorF vec(momentum.x, momentum.y, momentum.z);
      leakage += std::sqrt(vec.Mag2());
    }

    if (leakage > 5.)
      continue;

    float Edep = 0.;

    for (unsigned int iEdep = 0; iEdep < edepHits->size(); iEdep++)
      Edep += (*edepHits)[iEdep].getEnergy();

    tEdep->Fill(Edep);

    float en_S = 0.; float en_C = 0.;

    for (unsigned int idx = 0; idx < caloHits->size(); idx++) {
      const auto& caloHit = caloHits->at(idx);
      const auto& rawHit = rawHits->at(idx);
      const auto& digiHit = digiHits->at(idx);
      const auto& timeStruct = rawTimeStructs->at(idx);
      const auto& wavlenStruct = rawWavlenStructs->at(idx);
      const auto& waveform = digiWaveforms->at(idx);
      const auto& procTime = procTimes->at(idx);

      int type = caloHit.getType();
      float en = caloHit.getEnergy();
      int nhits = rawHit.getAmplitude();

      TH1F* tNhit = tNhit_dual->getHist(type);
      TH1F* tT = tT_dual->getHist(type);
      TH1F* tWav = tWav_dual->getHist(type);
      TH1F* tD = tD_dual->getHist(type);
      TH1F* tToa = tToa_dual->getHist(type);
      TH1F* tInt = tInt_dual->getHist(type);
      TH1F* tProc = tProc_dual->getHist(type);

      (type==0) ? en_S += en : en_C += en;

      tNhit->Fill(nhits);
      tToa->Fill(caloHit.getTime());

      if ( digiHit.getEnergy() > 0 )
        tInt->Fill(static_cast<double>(digiHit.getEnergy()));

      for (unsigned int bin = 0; bin < timeStruct.adcCounts_size(); bin++) {
        int con = timeStruct.getAdcCounts(bin);
        tT->Fill(timeStruct.getTime()+(static_cast<float>(bin)+0.5)*timeStruct.getInterval(),static_cast<float>(con));
      }

      for (unsigned int bin = 0; bin < wavlenStruct.adcCounts_size(); bin++) {
        int con = wavlenStruct.getAdcCounts(bin);
        tWav->Fill(wavlenStruct.getTime()+(static_cast<float>(bin)+0.5)*wavlenStruct.getInterval(),static_cast<float>(con));
      }

      for (unsigned int bin = 0; bin < waveform.amplitude_size(); bin++) {
        float con = waveform.getAmplitude(bin);
        tD->Fill(waveform.getTime()+(static_cast<float>(bin)+0.5)*waveform.getInterval(),con);
      }

      for (unsigned int bin = 0; bin < procTime.amplitude_size(); bin++) {
        float con = procTime.getAmplitude(bin);
        tProc->Fill(procTime.getTime()+(static_cast<float>(bin)+0.5)*procTime.getInterval(),con);
      }
    }

    E_Ss.push_back(en_S);
    E_Cs.push_back(en_C);

    tE_S->Fill(en_S);
    tE_C->Fill(en_C);
    tE_SC->Fill(en_C+en_S);
    tE_DR->Fill(functions::E_DR291(en_C,en_S));
  } // event loop

  std::string filenameStd = static_cast<std::string>(filenames.front());
  std::string extension = ".root";
  auto where = filenameStd.find(extension);
  if (where != std::string::npos) {
    filenameStd.erase(where, extension.length());
  }

  TString filename = static_cast<TString>(filenameStd);

  TCanvas* c = new TCanvas("c","");

  tEdep->Draw("Hist"); c->SaveAs(filename+"_Edep.png");

  TF1* grE_DR = new TF1("Efit","gaus",low,high); grE_DR->SetLineColor(kBlack);
  tE_DR->SetOption("p"); tE_DR->Fit(grE_DR,"R+&same");

  c->cd();
  tE_DR->SetTitle("");
  tE_DR->Draw(""); c->Update();
  TPaveStats* statsE_DR = (TPaveStats*)c->GetPrimitive("stats");
  statsE_DR->SetName("DR");
  statsE_DR->SetTextColor(kBlack);
  statsE_DR->SetX1NDC(.7);
  statsE_DR->SetY1NDC(.7); statsE_DR->SetY2NDC(1.);

  tE_S->SetTitle("");
  tE_S->Draw("Hist&sames"); c->Update();
  TPaveStats* statsE_S = (TPaveStats*)c->GetPrimitive("stats");
  statsE_S->SetName("Scint");
  statsE_S->SetTextColor(kRed);
  statsE_S->SetY1NDC(.5); statsE_S->SetY2NDC(.7);

  tE_C->Draw("Hist&sames"); c->Update();
  TPaveStats* statsE_C = (TPaveStats*)c->GetPrimitive("stats");
  statsE_C->SetName("Cerenkov");
  statsE_C->SetTextColor(kBlue);
  statsE_C->SetY1NDC(.3); statsE_C->SetY2NDC(.5);

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
  tE_SC->SetOption("p"); tE_SC->Fit(grE_SC,"R+&same");

  tE_SC->Draw(""); c->SaveAs(filename+"_Esum.png");

  TGraph* grSvsC = new TGraph(E_Ss.size(),&(E_Ss[0]),&(E_Cs[0]));
  grSvsC->SetTitle("SvsC;E_S;E_C");
  grSvsC->SetMarkerSize(0.5); grSvsC->SetMarkerStyle(20);
  grSvsC->GetXaxis()->SetLimits(0.,high);
  grSvsC->GetYaxis()->SetRangeUser(0.,high);
  grSvsC->SetMaximum(high);
  grSvsC->SetMinimum(0.);
  grSvsC->Draw("ap");
  c->SaveAs(filename+"_SvsC.png");

  tT_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_tC.png");
  tT_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_tS.png");
  tWav_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_wavC.png");
  tWav_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_wavS.png");
  tNhit_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_nhitC.png");
  tNhit_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_nhitS.png");

  tInt_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_intC.png");
  tInt_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_intS.png");

  tD_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_dC.png");
  tD_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_dS.png");

  tToa_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_toaC.png");
  tToa_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_toaS.png");

  tProc_dual->getHist(1)->Draw("Hist"); c->SaveAs(filename+"_procC.png");
  tProc_dual->getHist(0)->Draw("Hist"); c->SaveAs(filename+"_procS.png");

  TFile* validFile = new TFile(filename+"_validation.root","RECREATE");

  validFile->WriteTObject(tT_dual->getHist(1));
  validFile->WriteTObject(tT_dual->getHist(0));
  validFile->WriteTObject(tWav_dual->getHist(1));
  validFile->WriteTObject(tWav_dual->getHist(0));
  validFile->WriteTObject(tNhit_dual->getHist(1));
  validFile->WriteTObject(tNhit_dual->getHist(0));
  validFile->WriteTObject(tEdep);

  validFile->Close();
}
