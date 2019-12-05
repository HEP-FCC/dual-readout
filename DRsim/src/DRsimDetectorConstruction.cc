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

#include "WGR16PMTSD.hh"
#include "WGR16DetectorConstruction.hh"
#include "WGR16MagneticField.hh"
#include "WGR16CellParameterisation.hh"
#include "WGR16FilterParameterisation.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4AutoDelete.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Trap.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4ThreeVector.hh"
#include "G4IntersectionSolid.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"
#include "G4VisExtent.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"

#include "G4ReflectionFactory.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "geomdefs.hh"

#include <cmath>
#include <stdio.h>
#include <float.h>
#include <algorithm>

#include "dimensionB.hh"
#include "dimensionE.hh"
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal WGR16MagneticField* WGR16DetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager* WGR16DetectorConstruction::fFieldMgr = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16DetectorConstruction::WGR16DetectorConstruction()
: G4VUserDetectorConstruction(),
  fMessenger(0), fScoringVolume(0)
{
  // define commands for this class
  DefineCommands();

  clad_C_rMin = 0.49*mm;
  clad_C_rMax = 0.50*mm;
  clad_C_Dz   = 2.5*m;
  clad_C_Sphi = 0.;
  clad_C_Dphi = 2.*M_PI;

  core_C_rMin = 0.*mm;
  core_C_rMax = 0.49*mm;
  core_C_Dz   = 2.5*m;
  core_C_Sphi = 0.;
  core_C_Dphi = 2.*M_PI;

  clad_S_rMin = 0.485*mm;
  clad_S_rMax = 0.50*mm;
  clad_S_Dz   = 2.5*m;
  clad_S_Sphi = 0.;
  clad_S_Dphi = 2.*M_PI;

  core_S_rMin = 0.*mm;
  core_S_rMax = 0.485*mm;
  core_S_Dz   = 2.5*m;
  core_S_Sphi = 0.;
  core_S_Dphi = 2.*M_PI;
  PMTT = 0.3*mm;
  filterT = 0.01*mm;

  theta_unit=0;
  phi_unit=0;
  deltatheta=0;
  thetaofcenter=0;

  visAttrOrange = new G4VisAttributes(G4Colour(1.0,0.5,0.,1.0));
  visAttrOrange->SetVisibility(true);
  visAttrBlue = new G4VisAttributes(G4Colour(0.,0.,1.0,1.0));
  visAttrBlue->SetVisibility(true);
  visAttrGray = new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.3));
  visAttrGray->SetVisibility(true);
  visAttrGreen = new G4VisAttributes(G4Colour(0.3,0.7,0.3));
  visAttrGreen->SetVisibility(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16DetectorConstruction::~WGR16DetectorConstruction() {
  delete fMessenger;

  delete visAttrOrange;
  delete visAttrBlue;
  delete visAttrGray;
  delete visAttrGreen;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* WGR16DetectorConstruction::Construct() {
  G4cout << "Detector construct start" << G4endl;

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  checkOverlaps = false;

  ConstructMaterials();

  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;
  // n=number of nucleons in an isotope;

  G4int ncomponents, natoms;
  G4double fractionmass;

  G4Material* vac = G4Material::GetMaterial("G4_Galactic");
  cu  =new G4Material("Copper"   , z=29., a=63.546*g/mole, density=8.96*g/cm3);
  //G4Material* cu  =G4Material::GetMaterial("G4_Cu");

  G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
  G4Element* F  = new G4Element("Fluorine",symbol="F" , z= 9., a= 18.9984*g/mole);
  // G4Element* Si = new G4Element("Silicon" ,symbol="Si", z= 14., a= 28.09*g/mole);


  // define simple materials

  //--- for PMT Cathod ---
  // G4Material* Al = new G4Material("Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);
  SiWafer = new G4Material("Silicon", z=14., a=28.09*g/mole, density=2.33*g/cm3);

  ///--- for PMT Glass ---
  Glass = new G4Material("Glass", density=1.032*g/cm3,2);
  Glass->AddElement(C,91.533*perCent);
  Glass->AddElement(H,8.467*perCent);

  GlassLayer = new G4Material("GlassLayer", density=1.032*g/cm3,2);
  GlassLayer->AddElement(C,91.533*perCent);
  GlassLayer->AddElement(H,8.467*perCent);

  ///--- for scintillation fiber core ---
  G4Material* polystyrene = new G4Material("Polystyrene",density= 1.05*g/cm3, ncomponents=2);
  polystyrene->AddElement(C, natoms=8);
  polystyrene->AddElement(H, natoms=8);

  ///--- for cladding (scintillation fibers) ---
  G4Material* pmma_clad = new G4Material("PMMA_Clad",density= 1.19*g/cm3, ncomponents=3);
  pmma_clad->AddElement(C, natoms=5);
  pmma_clad->AddElement(H, natoms=8);
  pmma_clad->AddElement(O, natoms=2);

  ///--- for Cerenkov fiber core ---
  G4Material* pmma = new G4Material("PMMA",density= 1.19*g/cm3, ncomponents=3);
  pmma->AddElement(C, natoms=5);
  pmma->AddElement(H, natoms=8);
  pmma->AddElement(O, natoms=2);

  ///--- for cladding (Cerenkov fibers) ---
  G4Material* fluorinatedPolymer = new G4Material("Fluorinated_Polymer", density= 1.43*g/cm3, ncomponents=2);
  fluorinatedPolymer->AddElement(C,2);
  fluorinatedPolymer->AddElement(F,2);


  // define a material from elements.   case 2: mixture by fractional mass
  Air =	new G4Material("Air", density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);


  ///--- Material property tables for fiber materials ---
  G4MaterialPropertiesTable* mpAir;
  G4MaterialPropertiesTable* mpPS;
  G4MaterialPropertiesTable* mpPMMA;
  G4MaterialPropertiesTable* mpFS;
  G4MaterialPropertiesTable* mpGlass;
  G4MaterialPropertiesTable* mpPMTPC;
  G4MaterialPropertiesTable* mpFilter;
  G4MaterialPropertiesTable* mpFilterSurf;

  //--- Generate and add material properties table ---

  G4double PhotonEnergy[] = { // from 900nm to 300nm with 25nm step
    1.37760*eV, 1.41696*eV, 1.45864*eV, 1.50284*eV, 1.54980*eV, 1.59980*eV, 1.65312*eV, 1.71013*eV,
    1.77120*eV, 1.83680*eV, 1.90745*eV, 1.98375*eV, 2.06640*eV, 2.15625*eV, 2.25426*eV, 2.36160*eV,
    2.47968*eV, 2.61019*eV, 2.75520*eV, 2.91728*eV, 3.09960*eV, 3.30625*eV, 3.54241*eV, 3.81490*eV, 4.13281*eV
  };

  const G4int nEntries = sizeof(PhotonEnergy) / sizeof(G4double);
  //--- PMMA ---
  G4double RI_PMMA[nEntries] = {
    1.48329, 1.48355, 1.48392, 1.48434, 1.48467, 1.48515, 1.48569, 1.48628,
    1.48677, 1.48749, 1.48831, 1.48899, 1.49000, 1.49119, 1.49219, 1.49372,
    1.49552, 1.49766, 1.49953, 1.50252, 1.50519, 1.51000, 1.51518, 1.52182, 1.53055
  };
  G4double AbsLen_PMMA[nEntries] = {
    0.414*m, 0.543*m, 0.965*m, 2.171*m, 2.171*m, 3.341*m, 4.343*m, 1.448*m,
    4.343*m, 14.48*m, 21.71*m, 8.686*m, 28.95*m, 54.29*m, 43.43*m, 48.25*m,
    54.29*m, 48.25*m, 43.43*m, 28.95*m, 21.71*m, 4.343*m, 2.171*m, 0.869*m, 0.434*m
  };

  mpPMMA = new G4MaterialPropertiesTable();
  mpPMMA->AddProperty("RINDEX",PhotonEnergy,RI_PMMA,nEntries);
  mpPMMA->AddProperty("ABSLENGTH",PhotonEnergy,AbsLen_PMMA,nEntries);
  pmma->SetMaterialPropertiesTable(mpPMMA);
  pmma_clad->SetMaterialPropertiesTable(mpPMMA);

  //--- Fluorinated Polymer (FS) ---
  G4double RI_FluorinatedPolymer[nEntries]; std::fill_n(RI_FluorinatedPolymer, nEntries, 1.42);
  mpFS = new G4MaterialPropertiesTable();
  mpFS->AddProperty("RINDEX",PhotonEnergy,RI_FluorinatedPolymer,nEntries);
  fluorinatedPolymer->SetMaterialPropertiesTable(mpFS);

  G4double RI_PolyStyrene[nEntries] = {
    1.57483, 1.57568, 1.57644, 1.57726, 1.57817, 1.57916, 1.58026, 1.58148,
    1.58284, 1.58435, 1.58605, 1.58796, 1.59013, 1.59328, 1.59621, 1.59960,
    1.60251, 1.60824, 1.61229, 1.62032, 1.62858, 1.63886, 1.65191, 1.66888, 1.69165
  };
  G4double AbsLen_PolyStyrene[nEntries] = {
    2.714*m, 3.102*m, 3.619*m, 4.343*m, 5.791*m, 7.896*m, 4.343*m, 7.896*m,
    5.429*m, 36.19*m, 17.37*m, 36.19*m, 5.429*m, 28.95*m, 21.71*m, 14.48*m,
    12.41*m, 8.686*m, 7.238*m, 1.200*m, 0.200*m, 0.500*m, 0.200*m, 0.100*m, 0.100*m
  };
  G4double scintFast_PolyStyrene[nEntries] = {
    0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
    0.00, 0.00, 0.00, 0.00, 0.00, 0.03, 0.07, 0.13,
    0.33, 0.63, 1.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00
  };
  mpPS = new G4MaterialPropertiesTable();
  mpPS->AddProperty("RINDEX",PhotonEnergy,RI_PolyStyrene,nEntries);
  mpPS->AddProperty("ABSLENGTH",PhotonEnergy,AbsLen_PolyStyrene,nEntries);
  mpPS->AddProperty("FASTCOMPONENT",PhotonEnergy,scintFast_PolyStyrene,nEntries);
  mpPS->AddConstProperty("SCINTILLATIONYIELD",10./keV);
  mpPS->AddConstProperty("RESOLUTIONSCALE",1.0);
  mpPS->AddConstProperty("FASTTIMECONSTANT",2.8*ns);
  polystyrene->SetMaterialPropertiesTable(mpPS);
  polystyrene->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  //Glass
  G4double RI_Glass[nEntries]; std::fill_n(RI_Glass, nEntries, 1.52);
  G4double Abslength_Glass[nEntries]; std::fill_n(Abslength_Glass, nEntries, 420.*cm);
  mpGlass = new G4MaterialPropertiesTable();
  mpGlass->AddProperty("RINDEX",PhotonEnergy,RI_Glass,nEntries);
  mpGlass->AddProperty("ABSLENGTH",PhotonEnergy,Abslength_Glass,nEntries);
  Glass->SetMaterialPropertiesTable(mpGlass);
  GlassLayer->SetMaterialPropertiesTable(mpGlass);

  G4double RI_Air[nEntries]; std::fill_n(RI_Air, nEntries, 1.0);
  mpAir = new G4MaterialPropertiesTable();
  mpAir->AddProperty("RINDEX", PhotonEnergy, RI_Air, nEntries);
  Air->SetMaterialPropertiesTable(mpAir);

  //---Materials for Cerenkov fiber---
  clad_C_Material = fluorinatedPolymer;
  core_C_Material = pmma;

  //---Materials for Scintillation fiber---
  clad_S_Material = pmma_clad;
  core_S_Material = polystyrene;

  // filter

  filter = new G4Material("gelatin",density=1.27*g/cm3,ncomponents=4);
  filter->AddElement(C,natoms=102);
  filter->AddElement(H,natoms=151);
  filter->AddElement(N,natoms=31);
  filter->AddElement(O,natoms=39);

  G4double RI_gel[nEntries]; std::fill_n(RI_gel,nEntries,1.52);
  mpFilter = new G4MaterialPropertiesTable();
  mpFilter->AddProperty("RINDEX",PhotonEnergy,RI_gel,nEntries);
  filter->SetMaterialPropertiesTable(mpFilter);

  G4double filterEff[nEntries] = {
    1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000,
    1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 0.900, 0.734,
    0.568, 0.402, 0.296, 0.070, 0.000, 0.000, 0.000, 0.000, 0.000
  };
  G4double filterRef[nEntries]; std::fill_n(filterRef,nEntries,0.);
  mpFilterSurf = new G4MaterialPropertiesTable();
  mpFilterSurf->AddProperty("TRANSMITTANCE",PhotonEnergy,filterEff,nEntries);
  mpFilterSurf->AddProperty("REFLECTIVITY",PhotonEnergy,filterRef,nEntries);

  filterSurf = new G4OpticalSurface("filterSurf",glisur,polished,dielectric_dielectric);
  filterSurf->SetMaterialPropertiesTable(mpFilterSurf);

  //--- Material for PMT Photocathod ---
  PMTPC_Material = SiWafer;

  //--- Photocathod property ---
  G4double refl_PMT[nEntries]; std::fill_n(refl_PMT, nEntries, 0.);
  G4double eff_PMT[nEntries] = {
    0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10,
    0.11, 0.13, 0.15, 0.17, 0.19, 0.20, 0.22, 0.23,
    0.24, 0.25, 0.24, 0.23, 0.21, 0.20, 0.17, 0.14, 0.10
  };

  mpPMTPC = new G4MaterialPropertiesTable();
  mpPMTPC->AddProperty("REFLECTIVITY",PhotonEnergy,refl_PMT,nEntries);
  mpPMTPC->AddProperty("EFFICIENCY",PhotonEnergy,eff_PMT,nEntries);

  photocath_opsurf = new G4OpticalSurface("photocath_opsurf",glisur,polished,dielectric_metal);
  photocath_opsurf->SetMaterialPropertiesTable(mpPMTPC);

  G4VSolid* worldSolid = new G4Box("worldBox",10.*m,10.*m,10.*m);
  worldLogical = new G4LogicalVolume(worldSolid,vac,"worldLogical");
  G4VPhysicalVolume* worldPhysical = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,false,0,checkOverlaps);

  innerR = 1800.;
  tower_height = 2500.;
  NbOfBarrel = 52;
  NbOfEndcap = 40;
  NbOfZRot = 283;
  fulltheta = 0.;
  phi_unit = 2*M_PI/(G4double)NbOfZRot;

  fiber = new G4Tubs("fiber",0,clad_C_rMax,tower_height/2.,0*deg,360.*deg);// S is the same
  fiberC = new G4Tubs("fiberC",0,core_C_rMax,tower_height/2.,0*deg,360.*deg);
  fiberS = new G4Tubs("fiberS",0,core_S_rMax,tower_height/2.,0*deg,360.*deg);

  G4double deltatheta_endcap = deltatheta_barrel[NbOfBarrel-1];

  // barrel

  dimB = new dimensionB();
  dimB->SetInnerR(innerR);
  dimB->SetTower_height(tower_height);
  dimB->SetNumZRot(NbOfZRot);
  dimB->SetPMTT(PMTT+filterT);

  dimB->Rbool(1);
  thetaofcenter = 0.;
  fulltheta = 0.;
  Barrel(towerLogicalBR,PMTGLogicalBR,PMTfilterLogicalBR,PMTcellLogicalBR,PMTcathLogicalBR,fiberLogical_BR,fiberLogical_BR_);

  dimB->Rbool(0);
  thetaofcenter = 0.;
  fulltheta = 0.;
  Barrel(towerLogicalBL,PMTGLogicalBL,PMTfilterLogicalBL,PMTcellLogicalBL,PMTcathLogicalBL,fiberLogical_BL,fiberLogical_BL_);

  // endcap
  lastdeltatheta = deltatheta_endcap;
  dimE = new dimensionE();
  dimE->SetInnerR_new(3125.83);
  dimE->SetTower_height(tower_height);
  dimE->SetNumZRot(NbOfZRot);
  dimE->SetDeltaTheta(lastdeltatheta);
  dimE->SetPMTT(PMTT+filterT);

  fulltheta = 0.95717;
  dimE->Rbool(1);

  Endcap(towerLogicalER,PMTGLogicalER,PMTfilterLogicalER,PMTcellLogicalER,PMTcathLogicalER,fiberLogical_ER,fiberLogical_ER_);

  // endcap L
  fulltheta = 0.95717;
  dimE->Rbool(0);

  Endcap(towerLogicalEL,PMTGLogicalEL,PMTfilterLogicalEL,PMTcellLogicalEL,PMTcathLogicalEL,fiberLogical_EL,fiberLogical_EL_);

  delete dimE;
  delete dimB;

  G4cout << "DETECTORCONSTRUCTION finished!" << G4endl;
  return worldPhysical;
}

	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16DetectorConstruction::ConstructSDandField()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String PMTName = "SiPMSD";

  for(int i=0;i<NbOfBarrel;i++){
    PMTSDBR[i] = new WGR16PMTSD("BR"+std::to_string(i),"BRC"+std::to_string(i));
    SDman->AddNewDetector(PMTSDBR[i]);
    PMTcathLogicalBR[i]->SetSensitiveDetector(PMTSDBR[i]);
  }

  for(int i=0;i<NbOfBarrel;i++){
    PMTSDBL[i] = new WGR16PMTSD("BL"+std::to_string(i),"BLC"+std::to_string(i));
    SDman->AddNewDetector(PMTSDBL[i]);
    PMTcathLogicalBL[i]->SetSensitiveDetector(PMTSDBL[i]);
  }

  for(int i=0;i<NbOfEndcap;i++){
    PMTSDER[i] = new WGR16PMTSD("ER"+std::to_string(i),"ERC"+std::to_string(i));
    SDman->AddNewDetector(PMTSDER[i]);
    PMTcathLogicalER[i]->SetSensitiveDetector(PMTSDER[i]);
  }

  for(int i=0;i<NbOfEndcap;i++){
    PMTSDEL[i] = new WGR16PMTSD("EL"+std::to_string(i),"ELC"+std::to_string(i));
    SDman->AddNewDetector(PMTSDEL[i]);
    PMTcathLogicalEL[i]->SetSensitiveDetector(PMTSDEL[i]);
  }

  G4cout << "Ended construct SD" << G4endl;
}

void WGR16DetectorConstruction::Barrel(G4LogicalVolume* towerLogical[], G4LogicalVolume* PMTGLogical[], G4LogicalVolume* PMTfilterLogical[], G4LogicalVolume* PMTcellLogical[], G4LogicalVolume* PMTcathLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]) {

  for(int i=0;i<NbOfBarrel;i++) {
    thetaofcenter = fulltheta + deltatheta_barrel[i]/2.;
    dimB->SetDeltaTheta(deltatheta_barrel[i]);
    dimB->SetThetaOfCenter(thetaofcenter);
    dimB->CalBasic();
    dimB->Getpt(pt);
    towerName = setTowerName(dimB->GetRbool(), "B", i);

    tower = new G4Trap("TowerB",pt);
    towerLogical[i] = new G4LogicalVolume(tower,cu,towerName);

    dimB->Getpt_PMTG(pt);
    pmtg = new G4Trap("PMTGB",pt);
    PMTGLogical[i] = new G4LogicalVolume(pmtg,Air,towerName);

    for(int j=0;j<NbOfZRot;j++){
      new G4PVPlacement(dimB->GetRM(j),dimB->GetOrigin(j),towerLogical[i],towerName,worldLogical,false,j,checkOverlaps);
      new G4PVPlacement(dimB->GetRM(j),dimB->GetOrigin_PMTG(j),PMTGLogical[i],towerName,worldLogical,false,j,checkOverlaps);
    }

    dimB->Getpt(pt);
    fiberBarrel(i,deltatheta_barrel[i],towerLogical,fiberLogical,fiberLogical_);

    G4VSolid* SiPMlayerSolid = new G4Box("SiPMlayerSolid",numx*1.5/2.*mm,numy*1.5/2.*mm,PMTT/2.);
    G4LogicalVolume* SiPMlayerLogical = new G4LogicalVolume(SiPMlayerSolid,Air,"SiPMlayerLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,filterT/2.),SiPMlayerLogical,"SiPMlayerPhysical",PMTGLogical[i],false,0,checkOverlaps);

    G4VSolid* filterlayerSolid = new G4Box("filterlayerSolid",numx*1.5/2.*mm,numy*1.5/2.*mm,filterT/2.);
    G4LogicalVolume* filterlayerLogical = new G4LogicalVolume(filterlayerSolid,GlassLayer,"filterlayerLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,-PMTT/2.),filterlayerLogical,"filterlayerPhysical",PMTGLogical[i],false,0,checkOverlaps);

    G4VSolid* PMTcellSolid = new G4Box("PMTcellSolid",1.2/2.*mm,1.2/2.*mm,PMTT/2.);
    PMTcellLogical[i] = new G4LogicalVolume(PMTcellSolid,Glass,"PMTcellLogical");

    WGR16CellParameterisation* PMTcellParam = new WGR16CellParameterisation(numx,numy);
    G4PVParameterised* PMTcellPhysical = new G4PVParameterised("PMTcellPhysical",PMTcellLogical[i],SiPMlayerLogical,kXAxis,numx*numy,PMTcellParam);

    G4VSolid* PMTcathSolid = new G4Box("PMTcathSolid",1.2/2.*mm,1.2/2.*mm,0.01/2.*mm);
    PMTcathLogical[i] = new G4LogicalVolume(PMTcathSolid,SiWafer,"PMTcathLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,(PMTT-0.01)/2.*mm),PMTcathLogical[i],"PMTcathPhysical",PMTcellLogical[i],false,0,checkOverlaps);
    new G4LogicalSkinSurface("Photocath_surf",PMTcathLogical[i],photocath_opsurf);

    G4VSolid* filterSolid = new G4Box("filterSolid",1.2/2.*mm,1.2/2.*mm,filterT/2.);
    PMTfilterLogical[i] = new G4LogicalVolume(filterSolid,filter,"PMTfilterLogical");

    WGR16FilterParameterisation* filterParam = new WGR16FilterParameterisation(numx,numy);
    G4PVParameterised* filterPhysical = new G4PVParameterised("filterPhysical",PMTfilterLogical[i],filterlayerLogical,kXAxis,numx*numy/2,filterParam);
    new G4LogicalBorderSurface("filterSurf",filterPhysical,PMTcellPhysical,filterSurf);

    fulltheta = fulltheta+deltatheta_barrel[i];

    PMTcathLogical[i]->SetVisAttributes(visAttrGreen);
    PMTfilterLogical[i]->SetVisAttributes(visAttrOrange);
  }
}

void WGR16DetectorConstruction::Endcap(G4LogicalVolume* towerLogical[], G4LogicalVolume* PMTGLogical[], G4LogicalVolume* PMTfilterLogical[], G4LogicalVolume* PMTcellLogical[], G4LogicalVolume* PMTcathLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]) {

  for(int i=0;i<NbOfEndcap;i++) {
    thetaofcenter = fulltheta + lastdeltatheta/2.;
    dimE->SetThetaOfCenter(thetaofcenter);
    dimE->CalBasic();
    dimE->Getpt(pt);
    towerName = setTowerName(dimE->GetRbool(), "E", i);

    tower = new G4Trap("TowerE",pt);
    towerLogical[i] = new G4LogicalVolume(tower,cu,towerName);

    dimE->Getpt_PMTG(pt);
    pmtg = new G4Trap("PMTGE",pt);
    PMTGLogical[i] = new G4LogicalVolume(pmtg,Air,towerName);

    for(int j=0;j<NbOfZRot;j++){
      new G4PVPlacement(dimE->GetRM(j),dimE->GetOrigin(j),towerLogical[i],towerName,worldLogical,false,j,checkOverlaps);
      new G4PVPlacement(dimE->GetRM(j),dimE->GetOrigin_PMTG(j),PMTGLogical[i],towerName,worldLogical,false,j,checkOverlaps);
    }

    dimE->Getpt(pt);
    fiberEndcap(i,lastdeltatheta,towerLogical,fiberLogical,fiberLogical_);

    G4VSolid* SiPMlayerSolid = new G4Box("SiPMlayerSolid",numx*1.5/2.*mm,numy*1.5/2.*mm,PMTT/2.);
    G4LogicalVolume* SiPMlayerLogical = new G4LogicalVolume(SiPMlayerSolid,Air,"SiPMlayerLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,filterT/2.),SiPMlayerLogical,"SiPMlayerPhysical",PMTGLogical[i],false,0,checkOverlaps);

    G4VSolid* filterlayerSolid = new G4Box("filterlayerSolid",numx*1.5/2.*mm,numy*1.5/2.*mm,filterT/2.);
    G4LogicalVolume* filterlayerLogical = new G4LogicalVolume(filterlayerSolid,GlassLayer,"filterlayerLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,-PMTT/2.),filterlayerLogical,"filterlayerPhysical",PMTGLogical[i],false,0,checkOverlaps);

    G4VSolid* PMTcellSolid = new G4Box("PMTcellSolid",1.2/2.*mm,1.2/2.*mm,PMTT/2.);
    PMTcellLogical[i] = new G4LogicalVolume(PMTcellSolid,Glass,"PMTcellLogical");

    WGR16CellParameterisation* PMTcellParam = new WGR16CellParameterisation(numx,numy);
    G4PVParameterised* PMTcellPhysical = new G4PVParameterised("PMTcellPhysical",PMTcellLogical[i],SiPMlayerLogical,kXAxis,numx*numy,PMTcellParam);

    G4VSolid* PMTcathSolid = new G4Box("PMTcathSolid",1.2/2.*mm,1.2/2.*mm,0.01/2.*mm);
    PMTcathLogical[i] = new G4LogicalVolume(PMTcathSolid,SiWafer,"PMTcathLogical");
    new G4PVPlacement(0,G4ThreeVector(0.,0.,(PMTT-0.01)/2.*mm),PMTcathLogical[i],"PMTcathPhysical",PMTcellLogical[i],false,0,checkOverlaps);
    new G4LogicalSkinSurface("Photocath_surf",PMTcathLogical[i],photocath_opsurf);

    G4VSolid* filterSolid = new G4Box("filterSolid",1.2/2.*mm,1.2/2.*mm,filterT/2.);
    PMTfilterLogical[i] = new G4LogicalVolume(filterSolid,filter,"PMTfilterLogical");

    WGR16FilterParameterisation* filterParam = new WGR16FilterParameterisation(numx,numy);
    G4PVParameterised* filterPhysical = new G4PVParameterised("filterPhysical",PMTfilterLogical[i],filterlayerLogical,kXAxis,numx*numy/2,filterParam);
    new G4LogicalBorderSurface("filterSurf",filterPhysical,PMTcellPhysical,filterSurf);

    fulltheta = fulltheta+lastdeltatheta;

    PMTcathLogical[i]->SetVisAttributes(visAttrGreen);
    PMTfilterLogical[i]->SetVisAttributes(visAttrOrange);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16DetectorConstruction::ConstructMaterials() {
  G4NistManager* nistManager = G4NistManager::Instance();

  // Vacuum "Galactic"
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_Cu");

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16DetectorConstruction::DefineCommands() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16DetectorConstruction::fiberBarrel(G4int i, G4double deltatheta_,G4LogicalVolume* towerLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]) {

  fFiberX.clear();
  fFiberY.clear();
  fFiberWhich.clear();

  v1 = dimB->GetV1();
  v2 = dimB->GetV2();
  v3 = dimB->GetV3();
  v4 = dimB->GetV4();

  innerSide_half = dimB->GetInnerR_new()*tan(deltatheta_/2.);
  outerSide_half = (dimB->GetInnerR_new()+tower_height)*tan(deltatheta_/2.);

  numx = (int)(((v4.getX()*tan(phi_unit/2.)*2)-1.2*mm)/(1.5*mm)) + 1;
  numy = (int)((outerSide_half*2-1.2*mm)/(1.5*mm)) + 1;

  G4bool fWhich = false;
  for (int j = 0; j < numy; j++) {
    for (int k = 0; k < numx; k++) {
      G4float fX = -1.5*mm*(numx/2) + k*1.5*mm + ( numx%2==0 ? 0.75*mm : 0 );
      G4float fY = -1.5*mm*(numy/2) + j*1.5*mm + ( numy%2==0 ? 0.75*mm : 0 );
      fWhich = !fWhich;
      fFiberX.push_back(fX);
      fFiberY.push_back(fY);
      fFiberWhich.push_back(fWhich);
    }
    if ( numx%2==0 ) { fWhich = !fWhich; }
  }

  for (unsigned int j = 0; j<fFiberX.size();j++) {

    if ( !fFiberWhich.at(j) ) { //c fibre

      intersect = new G4IntersectionSolid("fiber_",fiber,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical[i].push_back(new G4LogicalVolume(intersect,clad_C_Material,name));
      new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberLogical[i].at(j),name,towerLogical[i],false,j,checkOverlaps);

      intersect_ = new G4IntersectionSolid("fiber_",fiberC,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical_[i].push_back(new G4LogicalVolume(intersect_,core_C_Material,name));
      new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberLogical_[i].at(j),name,fiberLogical[i].at(j),false,j,checkOverlaps);

      fiberLogical[i].at(j)->SetVisAttributes(visAttrGray);
      fiberLogical_[i].at(j)->SetVisAttributes(visAttrBlue);
    } else { // s fibre
      intersect = new G4IntersectionSolid("fiber_",fiber,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical[i].push_back(new G4LogicalVolume(intersect,clad_S_Material,name));
      new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberLogical[i].at(j),name,towerLogical[i],false,j,checkOverlaps);

      intersect_ = new G4IntersectionSolid("fiber_",fiberS,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical_[i].push_back(new G4LogicalVolume(intersect_,core_S_Material,name));
      new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberLogical_[i].at(j),name,fiberLogical[i].at(j),false,j,checkOverlaps);

      fiberLogical[i].at(j)->SetVisAttributes(visAttrGray);
      fiberLogical_[i].at(j)->SetVisAttributes(visAttrOrange);
    }
  }
}

void WGR16DetectorConstruction::fiberEndcap(G4int i, G4double deltatheta_, G4LogicalVolume* towerLogical[], std::vector<G4LogicalVolume*> fiberLogical[], std::vector<G4LogicalVolume*> fiberLogical_[]) {

  fFiberX.clear();
  fFiberY.clear();
  fFiberWhich.clear();

  v1 = dimE->GetV1();
  v2 = dimE->GetV2();
  v3 = dimE->GetV3();
  v4 = dimE->GetV4();

  innerSide_half = dimE->GetInnerR_new()*tan(deltatheta_/2.);
  outerSide_half = (dimE->GetInnerR_new()+tower_height)*tan(deltatheta_/2.);

  numx = (int)(((v4.getX()*tan(phi_unit/2.)*2)-1.2*mm)/(1.5*mm)) + 1;
  numy = (int)((outerSide_half*2-1.2*mm)/(1.5*mm)) + 1;

  G4bool fWhich = false;
  for (int j = 0; j < numy; j++) {
    for (int k = 0; k < numx; k++) {
      G4float fX = -1.5*mm*(numx/2) + k*1.5*mm + ( numx%2==0 ? 0.75*mm : 0 );
      G4float fY = -1.5*mm*(numy/2) + j*1.5*mm + ( numy%2==0 ? 0.75*mm : 0 );
      fWhich = !fWhich;
      fFiberX.push_back(fX);
      fFiberY.push_back(fY);
      fFiberWhich.push_back(fWhich);
    }
    if ( numx%2==0 ) { fWhich = !fWhich; }
  }

  for (unsigned int j = 0; j<fFiberX.size();j++) {
    // determine z value for center of fibre

    if ( !fFiberWhich.at(j) ) { //c fibre
      intersect = new G4IntersectionSolid("fiber_",fiber,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical[i].push_back(new G4LogicalVolume(intersect,clad_C_Material,name));
      new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberLogical[i].at(j),name,towerLogical[i],false,j,checkOverlaps);

      intersect_ = new G4IntersectionSolid("fiber_",fiberC,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical_[i].push_back(new G4LogicalVolume(intersect_,core_C_Material,name));
      new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberLogical_[i].at(j),name,fiberLogical[i].at(j),false,j,checkOverlaps);

      fiberLogical[i].at(j)->SetVisAttributes(visAttrGray);
      fiberLogical_[i].at(j)->SetVisAttributes(visAttrBlue);
    } else { // s fibre
      intersect = new G4IntersectionSolid("fiber_",fiber,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical[i].push_back(new G4LogicalVolume(intersect,clad_S_Material,name));
      new G4PVPlacement(0,G4ThreeVector(fFiberX.at(j),fFiberY.at(j),0),fiberLogical[i].at(j),name,towerLogical[i],false,j,checkOverlaps);

      intersect_ = new G4IntersectionSolid("fiber_",fiberS,tower,0,G4ThreeVector(-fFiberX.at(j),-fFiberY.at(j),0.));
      fiberLogical_[i].push_back(new G4LogicalVolume(intersect_,core_S_Material,name));
      new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),fiberLogical_[i].at(j),name,fiberLogical[i].at(j),false,j,checkOverlaps);

      fiberLogical[i].at(j)->SetVisAttributes(visAttrGray);
      fiberLogical_[i].at(j)->SetVisAttributes(visAttrOrange);
    }
  }
}
