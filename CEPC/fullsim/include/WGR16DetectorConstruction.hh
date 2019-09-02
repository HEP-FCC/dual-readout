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

#ifndef WGR16DetectorConstruction_h
#define WGR16DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include <string.h>
#include <vector>
#include "dimensionB.hh"
#include "dimensionE.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4VSensitiveDetector.hh"
#include "G4OpticalSurface.hh"

using namespace std;
class WGR16MagneticField;
class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4GenericMessenger;
class G4VPhysicalVolume;

/// Detector construction

class WGR16DetectorConstruction : public G4VUserDetectorConstruction {
public:
  WGR16DetectorConstruction();
  virtual ~WGR16DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void ConstructMaterials();
  void Barrel(G4LogicalVolume* towerLogical[], G4LogicalVolume* PMTGLogical[], G4LogicalVolume* PMTfilterLogical[], G4LogicalVolume* PMTcellLogical[], G4LogicalVolume* PMTcathLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]);
  void fiberBarrel(G4int i, G4double deltatheta_, G4LogicalVolume* towerLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]);
  void Endcap(G4LogicalVolume* towerLogical[], G4LogicalVolume* PMTGLogical[], G4LogicalVolume* PMTfilterLogical[], G4LogicalVolume* PMTcellLogical[], G4LogicalVolume* PMTcathLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]);
  void fiberEndcap(G4int i, G4double deltatheta, G4LogicalVolume* towerLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]);

  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

private:
  void DefineCommands();

  G4bool checkOverlaps;
  G4GenericMessenger* fMessenger;

  static G4ThreadLocal WGR16MagneticField* fMagneticField;
  static G4ThreadLocal G4FieldManager* fFieldMgr;

  G4VisAttributes* visAttrOrange;
  G4VisAttributes* visAttrBlue;
  G4VisAttributes* visAttrGray;
  G4VisAttributes* visAttrGreen;

  G4double innerR;
  G4double tower_height;

  G4int NbOfBarrel;
  G4int NbOfEndcap;
  G4int NbOfZRot;

  G4double theta_unit;
  G4double phi_unit;

  G4double deltatheta;
  G4double thetaofcenter;
  G4double fulltheta;
  G4double lastdeltatheta;

  G4ThreeVector pt[8]={G4ThreeVector()};

  G4double PMTT;
  G4double filterT;

  dimensionB* dimB;
  dimensionE* dimE;

  char name[20];
  G4String towerName;
  G4Trap* tower;
  G4Trap* pmtg;
  G4Trap* pmtcath;
  G4Tubs* fiber;
  G4Tubs* fiber_S;
  G4Tubs* fiber_C;
  G4Tubs* fiberS;
  G4Tubs* fiberC;
  G4VSolid* intersect;
  G4VSolid* intersect_;

  G4LogicalVolume* towerLogicalBR[52];
  G4LogicalVolume* towerLogicalBL[52];

  G4LogicalVolume* towerLogicalER[40];
  G4LogicalVolume* towerLogicalEL[40];

  G4LogicalVolume* PMTGLogicalBR[52];
  G4LogicalVolume* PMTGLogicalBL[52];

  G4LogicalVolume* PMTGLogicalER[40];
  G4LogicalVolume* PMTGLogicalEL[40];

  G4LogicalVolume* PMTcathLogicalBR[52];
  G4LogicalVolume* PMTcellLogicalBR[52];
  G4LogicalVolume* PMTfilterLogicalBR[52];

  G4LogicalVolume* PMTcathLogicalBL[52];
  G4LogicalVolume* PMTcellLogicalBL[52];
  G4LogicalVolume* PMTfilterLogicalBL[52];

  G4LogicalVolume* PMTcathLogicalER[40];
  G4LogicalVolume* PMTcellLogicalER[40];
  G4LogicalVolume* PMTfilterLogicalER[40];

  G4LogicalVolume* PMTcathLogicalEL[40];
  G4LogicalVolume* PMTcellLogicalEL[40];
  G4LogicalVolume* PMTfilterLogicalEL[40];

  vector<G4LogicalVolume*> fiberLogical_BR[52];
  vector<G4LogicalVolume*> fiberLogical_BR_[52];
  vector<G4LogicalVolume*> fiberLogical_BL[52];
  vector<G4LogicalVolume*> fiberLogical_BL_[52];

  vector<G4LogicalVolume*> fiberLogical_ER[40];
  vector<G4LogicalVolume*> fiberLogical_ER_[40];
  vector<G4LogicalVolume*> fiberLogical_EL[40];
  vector<G4LogicalVolume*> fiberLogical_EL_[40];

  G4VSensitiveDetector* PMTSDBR[52];
  G4VSensitiveDetector* PMTSDBL[52];

  G4VSensitiveDetector* PMTSDER[40];
  G4VSensitiveDetector* PMTSDEL[40];

  G4double clad_C_rMin;
  G4double clad_C_rMax;
  G4double clad_C_Dz  ;
  G4double clad_C_Sphi;
  G4double clad_C_Dphi;

  G4double core_C_rMin;
  G4double core_C_rMax;
  G4double core_C_Dz  ;
  G4double core_C_Sphi;
  G4double core_C_Dphi;

  G4double clad_S_rMin;
  G4double clad_S_rMax;
  G4double clad_S_Dz  ;
  G4double clad_S_Sphi;
  G4double clad_S_Dphi;

  G4double core_S_rMin;
  G4double core_S_rMax;
  G4double core_S_Dz  ;
  G4double core_S_Sphi;
  G4double core_S_Dphi;

  //---Materials for Cerenkov fiber---
  G4Material *clad_C_Material;
  G4Material *core_C_Material;

  //---Materials for Scintillation fiber---
  G4Material *clad_S_Material;
  G4Material *core_S_Material;

  //--- Material for PMT Photocathod ---
  G4Material *PMTPC_Material;

  std::vector<G4float> fFiberX;
  std::vector<G4float> fFiberY;
  std::vector<G4bool> fFiberWhich;

  int numx;
  int numy;

  G4ThreeVector v1;
  G4ThreeVector v2;
  G4ThreeVector v3;
  G4ThreeVector v4;

  G4double innerSide_half;
  G4double innerSide_half_;
  G4double outerSide_half;
  G4double outerSide_half_;

  G4double deltatheta_barrel[52] = {
    0.02222,0.02220,0.02217,0.02214,0.02209,0.02203,0.02196,0.02188,0.02179,0.02169,
    0.02158,0.02146,0.02133,0.02119,0.02105,0.02089,0.02073,0.02056,0.02039,0.02020,
    0.02002,0.01982,0.01962,0.01941,0.01920,0.01898,0.01876,0.01854,0.01831,0.01808,
    0.01785,0.01761,0.01738,0.01714,0.01689,0.01665,0.01641,0.01616,0.01592,0.01567,
    0.01543,0.01518,0.01494,0.01470,0.01445,0.01421,0.01397,0.01373,0.01350,0.01326,
    0.01303,0.01280
  }; //apply the significance digit

  G4Material* cu;
  G4Material* Glass;
  G4Material* GlassLayer;
  G4Material* SiWafer;
  G4Material* Air;
  G4Material* filter;

  G4LogicalVolume* worldLogical;
  G4OpticalSurface* photocath_opsurf;
  G4OpticalSurface* filterSurf;

  G4String setTowerName(bool rbool, G4String BorE, int i) {
    if (rbool) return "T" + BorE + "R" + std::to_string(i);
    else return "T" + BorE + "L" + std::to_string(i);
  }

protected:
  G4LogicalVolume* fScoringVolume;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
