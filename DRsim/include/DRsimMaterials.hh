#ifndef DRsimMaterials_h
#define DRsimMaterials_h 1

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

class DRsimMaterials {
public:
  virtual ~DRsimMaterials();
  static DRsimMaterials* GetInstance();
  G4Material* GetMaterial(const G4String);
  G4OpticalSurface* GetOpticalSurface(const G4String);

private:
  DRsimMaterials();
  void CreateMaterials();

  static DRsimMaterials* fInstance;
  G4NistManager* fNistMan;
  G4Material* fVacuum;
  G4Material* fAir;
  G4Material* fFluoPoly;
  G4Material* fPMMA;
  G4Material* fPS;
  G4Material* fCu;
  G4Material* fSi;
  G4Material* fGlass;
  G4Material* fGelatin;
  G4OpticalSurface* fSiPMSurf;
  G4OpticalSurface* fFilterSurf;
};

#endif
