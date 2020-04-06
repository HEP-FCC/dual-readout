#include "DRsimMaterials.hh"
#include "G4SystemOfUnits.hh"

#include <stdio.h>
#include <algorithm>
#include <cmath>

DRsimMaterials* DRsimMaterials::fInstance = 0;

DRsimMaterials::DRsimMaterials() {
  fNistMan = G4NistManager::Instance();
  CreateMaterials();
}

DRsimMaterials::~DRsimMaterials() {}

DRsimMaterials* DRsimMaterials::GetInstance() {
  if (fInstance==0) fInstance = new DRsimMaterials();

  return fInstance;
}

G4Material* DRsimMaterials::GetMaterial(const G4String matName) {
  G4Material* mat = fNistMan->FindOrBuildMaterial(matName);

  if (!mat) mat = G4Material::GetMaterial(matName);
  if (!mat) {
    std::ostringstream o;
    o << "Material " << matName << " not found!";
    G4Exception("DRsimMaterials::GetMaterial","",FatalException,o.str().c_str());
  }

  return mat;
}

G4OpticalSurface* DRsimMaterials::GetOpticalSurface(const G4String surfName) {
  if (surfName=="SiPMSurf") return fSiPMSurf;
  else if (surfName=="FilterSurf") return fFilterSurf;
  else {
    std::ostringstream o;
    o << "OpticalSurface " << surfName << " not found!";
    G4Exception("DRsimMaterials::GetOpticalSurface","",FatalException,o.str().c_str());
  }

  return nullptr;
}

void DRsimMaterials::CreateMaterials() {
  fNistMan->FindOrBuildMaterial("G4_Galactic");
  fNistMan->FindOrBuildMaterial("G4_AIR");
  fNistMan->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE");

  G4String symbol;
  G4double a, z, density;
  G4int ncomponents, natoms;
  G4Element* H  = new G4Element("Hydrogen",symbol="H" , z=1., a=1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z=6., a=12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen",symbol="N" , z=7., a=14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z=8., a=16.00*g/mole);
  G4Element* F  = new G4Element("Fluorine",symbol="F" , z=9., a=18.9984*g/mole);

  fCu = new G4Material("Copper", z=29., a=63.546*g/mole, density=8.96*g/cm3);
  fSi = new G4Material("Silicon", z=14., a=28.09*g/mole, density=2.33*g/cm3);

  fVacuum = G4Material::GetMaterial("G4_Galactic");
  fAir = G4Material::GetMaterial("G4_AIR");
  fPVC = G4Material::GetMaterial("G4_POLYVINYL_CHLORIDE");

  fFluoPoly = new G4Material("FluorinatedPolymer", density=1.43*g/cm3, ncomponents=2);
  fFluoPoly->AddElement(C, 2);
  fFluoPoly->AddElement(F, 2);

  fGlass = new G4Material("Glass", density=1.032*g/cm3, 2);
  fGlass->AddElement(C, 91.533*perCent);
  fGlass->AddElement(H, 8.467*perCent);

  fPS = new G4Material("Polystyrene", density=1.05*g/cm3, ncomponents=2);
  fPS->AddElement(C, natoms=8);
  fPS->AddElement(H, natoms=8);

  fPMMA = new G4Material("PMMA", density= 1.19*g/cm3, ncomponents=3);
  fPMMA->AddElement(C, natoms=5);
  fPMMA->AddElement(H, natoms=8);
  fPMMA->AddElement(O, natoms=2);

  fGelatin = new G4Material("Gelatin", density=1.27*g/cm3, ncomponents=4);
  fGelatin->AddElement(C, natoms=102);
  fGelatin->AddElement(H, natoms=151);
  fGelatin->AddElement(N, natoms=31);
  fGelatin->AddElement(O, natoms=39);

  G4MaterialPropertiesTable* mpAir;
  G4MaterialPropertiesTable* mpPS;
  G4MaterialPropertiesTable* mpPMMA;
  G4MaterialPropertiesTable* mpFluoPoly;
  G4MaterialPropertiesTable* mpGlass;
  G4MaterialPropertiesTable* mpSiPM;
  G4MaterialPropertiesTable* mpFilter;
  G4MaterialPropertiesTable* mpFilterSurf;

  G4double opEn[] = {
    1.37760*eV, 1.45864*eV, 1.54980*eV, // 900, 850, 800
    1.65312*eV, 1.71013*eV, 1.77120*eV, 1.83680*eV, 1.90745*eV, 1.98375*eV, // 750, 725, 700, 675, 650, 625
    2.06640*eV, 2.10143*eV, 2.13766*eV, 2.17516*eV, 2.21400*eV, 2.25426*eV, 2.29600*eV, 2.33932*eV, 2.38431*eV, 2.43106*eV, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    2.47968*eV, 2.53029*eV, 2.58300*eV, 2.63796*eV, 2.69531*eV, 2.75520*eV, 2.81782*eV, 2.88335*eV, 2.95200*eV, 3.09960*eV, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    3.54241*eV, 4.13281*eV // 350, 300
  };

  const G4int nEnt = sizeof(opEn) / sizeof(G4double);

  G4double RI_Air[nEnt]; std::fill_n(RI_Air,nEnt,1.0);
  mpAir = new G4MaterialPropertiesTable();
  mpAir->AddProperty("RINDEX",opEn,RI_Air,nEnt);
  fAir->SetMaterialPropertiesTable(mpAir);

  G4double RI_PMMA[nEnt] = {
    1.4852, 1.4859, 1.4867, // 900, 850, 800
    1.4877, 1.4882, 1.4888, 1.4895, 1.4903, 1.4911, // 750, 725, 700, 675, 650, 625
    1.4920, 1.4924, 1.4929, 1.4933, 1.4938, 1.4943, 1.4948, 1.4954, 1.4960, 1.4966, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    1.4973, 1.4981, 1.4989, 1.4997, 1.5006, 1.5016, 1.5026, 1.5038, 1.5050, 1.5052, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    1.5152, 1.5306 // 350, 300
  };
  G4double AbsLen_PMMA[nEnt] = {
    0.414*m, 0.965*m, 2.171*m, // 900, 850, 800
    4.343*m, 1.448*m, 4.343*m, 14.48*m, 21.71*m, 8.686*m, // 750, 725, 700, 675, 650, 625
    39.48*m, 48.25*m, 54.29*m, 57.91*m, 54.29*m, 33.40*m, 31.02*m, 43.43*m, 43.43*m, 41.36*m, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510,
    39.48*m, 37.76*m, 36.19*m, 36.19*m, 33.40*m, 31.02*m, 28.95*m, 25.55*m, 24.13*m, 21.71*m, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400,
    2.171*m, 0.434*m // 350, 300
  };
  mpPMMA = new G4MaterialPropertiesTable();
  mpPMMA->AddProperty("RINDEX",opEn,RI_PMMA,nEnt);
  mpPMMA->AddProperty("ABSLENGTH",opEn,AbsLen_PMMA,nEnt);
  fPMMA->SetMaterialPropertiesTable(mpPMMA);

  G4double RI_FluoPoly[nEnt]; std::fill_n(RI_FluoPoly, nEnt, 1.42);
  mpFluoPoly = new G4MaterialPropertiesTable();
  mpFluoPoly->AddProperty("RINDEX",opEn,RI_FluoPoly,nEnt);
  fFluoPoly->SetMaterialPropertiesTable(mpFluoPoly);

  G4double RI_PS[nEnt] = {
    1.5749, 1.5764, 1.5782, // 900, 850, 800
    1.5803, 1.5815, 1.5829, 1.5845, 1.5862, 1.5882, // 750, 725, 700, 675, 650, 625
    1.5904, 1.5914, 1.5924, 1.5935, 1.5947, 1.5959, 1.5972, 1.5986, 1.6000, 1.6016 , // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    1.6033, 1.6051, 1.6070, 1.6090, 1.6112, 1.6136, 1.6161, 1.6170, 1.6230, 1.62858, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    1.65191, 1.69165 // 400, 350, 300
  };
  G4double AbsLen_PS[nEnt] = {
    2.714*m, 3.619*m, 5.791*m, // 900, 850, 800
    4.343*m, 7.896*m, 5.429*m, 36.19*m, 17.37*m, 36.19*m, // 750, 725, 700, 675, 650, 625
    5.429*m, 13.00*m, 14.50*m, 16.00*m, 18.00*m, 16.50*m, 17.00*m, 14.00*m, 16.00*m, 15.00*m, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    14.50*m, 13.00*m, 12.00*m, 10.00*m, 8.000*m, 7.238*m, 4.000*m, 1.200*m, 0.500*m, 0.200*m, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.200*m, 0.100*m // 350, 300
  };
  G4double scintFast_PS[nEnt] = {
    0.0   , 0.0   , 0.0   , // 900, 850, 800
    0.0   , 0.0   , 0.0   , 0.0   , 0.0003, 0.0008, // 750, 725, 700, 675, 650, 625
    0.0032, 0.0057, 0.0084, 0.0153, 0.0234, 0.0343, 0.0604, 0.0927, 0.1398, 0.2105, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.2903, 0.4122, 0.5518, 0.7086, 0.8678, 1.0000, 0.8676, 0.2311, 0.0033, 0.0012, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.00  , 0.00 // 350, 300
  };
  mpPS = new G4MaterialPropertiesTable();
  mpPS->AddProperty("RINDEX",opEn,RI_PS,nEnt);
  mpPS->AddProperty("ABSLENGTH",opEn,AbsLen_PS,nEnt);
  mpPS->AddProperty("FASTCOMPONENT",opEn,scintFast_PS,nEnt);
  mpPS->AddConstProperty("SCINTILLATIONYIELD",13.9/keV);
  mpPS->AddConstProperty("RESOLUTIONSCALE",1.0);
  mpPS->AddConstProperty("FASTTIMECONSTANT",2.8*ns);
  fPS->SetMaterialPropertiesTable(mpPS);
  fPS->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  G4double RI_Glass[nEnt]; std::fill_n(RI_Glass, nEnt, 1.52);
  G4double Abslength_Glass[nEnt]; std::fill_n(Abslength_Glass, nEnt, 420.*cm);
  mpGlass = new G4MaterialPropertiesTable();
  mpGlass->AddProperty("RINDEX",opEn,RI_Glass,nEnt);
  mpGlass->AddProperty("ABSLENGTH",opEn,Abslength_Glass,nEnt);
  fGlass->SetMaterialPropertiesTable(mpGlass);

  G4double refl_SiPM[nEnt]; std::fill_n(refl_SiPM, nEnt, 0.);
  G4double eff_SiPM[nEnt] = {
    0.03, 0.05 , 0.07 , // 900, 850, 800
    0.09, 0.10 , 0.11 , 0.13 , 0.15, 0.17 , // 750, 725, 700, 675, 650, 625
    0.18, 0.188, 0.195, 0.203, 0.21, 0.218, 0.225, 0.233, 0.24, 0.245, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.25, 0.255, 0.26 , 0.255, 0.25, 0.245, 0.24 , 0.23 , 0.22, 0.21 , // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.17, 0.10 // 350, 300
  };
  mpSiPM = new G4MaterialPropertiesTable();
  mpSiPM->AddProperty("REFLECTIVITY",opEn,refl_SiPM,nEnt);
  mpSiPM->AddProperty("EFFICIENCY",opEn,eff_SiPM,nEnt);
  fSiPMSurf = new G4OpticalSurface("SiPMSurf",glisur,polished,dielectric_metal);
  fSiPMSurf->SetMaterialPropertiesTable(mpSiPM);

  G4double filterEff[nEnt] = { // Kodak Wratten #9
    0.903, 0.903, 0.903, // 900, 850, 800
    0.903, 0.903, 0.903, 0.902, 0.901, 0.898, // 750, 725, 700, 675, 650, 625
    0.895, 0.893, 0.891, 0.888, 0.883, 0.870, 0.838, 0.760, 0.62 , 0.488, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.345, 0.207, 0.083, 0.018, 0.0  , 0.0  , 0.0  , 0.0  , 0.0  , 0.0  , // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.0  , 0.0 // 350, 300
  };
  G4double filterRef[nEnt]; std::fill_n(filterRef,nEnt,0.);
  G4double RI_gel[nEnt]; std::fill_n(RI_gel,nEnt,1.52);
  mpFilter = new G4MaterialPropertiesTable();
  mpFilter->AddProperty("RINDEX",opEn,RI_gel,nEnt);
  fGelatin->SetMaterialPropertiesTable(mpFilter);
  mpFilterSurf = new G4MaterialPropertiesTable();
  mpFilterSurf->AddProperty("TRANSMITTANCE",opEn,filterEff,nEnt);
  mpFilterSurf->AddProperty("REFLECTIVITY",opEn,filterRef,nEnt);
  fFilterSurf = new G4OpticalSurface("FilterSurf",glisur,polished,dielectric_dielectric);
  fFilterSurf->SetMaterialPropertiesTable(mpFilterSurf);
}
