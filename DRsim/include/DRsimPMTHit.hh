///////////////////////////////////////////////////////////////////////
//// <CEPC>                                                        ////
//// Wedge Geometry for Dual-reaout calorimter                     ////
////                                                               ////
//// Original Author: Mr.Jo Hyunsuk, Kyunpook National University  ////
////                  Sanghyun Ko, Seoul National University       ////
//// E-Mail: hyunsuk.jo@cern.ch	                                   ////
////         sang.hyun.ko@cern.ch                                  ////
////                                                               ////
///////////////////////////////////////////////////////////////////////
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#ifndef WGR16PMTHit_h
#define WGR16PMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

class WGR16PMTHit : public G4VHit {
public:
  WGR16PMTHit(G4int wavBin, G4int timeBin);
  WGR16PMTHit(const WGR16PMTHit &right);
  virtual ~WGR16PMTHit();

  const WGR16PMTHit& operator=(const WGR16PMTHit &right);
  int operator==(const WGR16PMTHit &right) const;

  void *operator new(size_t);
  void operator delete(void* aHit);

  void Draw();
  void Print();

  void photonCount() { fPhotons++; }
  G4int GetPhotonCount() const { return fPhotons; }

  void SetPMTnum(G4int n) { fPMTnum = n; }
  G4int GetPMTnum() const { return fPMTnum; }

  void SetTowerIEta(G4int eta) { fTowerIEta = eta; }
  G4int GetTowerIEta() const { return fTowerIEta; }

  void SetTowerIPhi(G4int phi) { fTowerIPhi = phi; }
  G4int GetTowerIPhi() const { return fTowerIPhi; }

  void wavelenCount(G4int bin) { fWavelen.at(bin)++; }
  G4int GetWavelenCount(G4int bin) const { return fWavelen.at(bin); }

  void timeCount(G4int bin) { fTimeCount.at(bin)++; }
  G4int GetTimeCount(G4int bin) const { return fTimeCount.at(bin); }

private:
  G4int fPMTnum;
  G4int fPhotons;
  G4int fTowerIEta;
  G4int fTowerIPhi;
  std::vector<G4int> fWavelen;
  std::vector<G4int> fTimeCount;
  G4int fWavBin;
  G4int fTimeBin;
};

typedef G4THitsCollection<WGR16PMTHit> WGR16PMTHitsCollection;
extern G4ThreadLocal G4Allocator<WGR16PMTHit>* WGR16PMTHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
