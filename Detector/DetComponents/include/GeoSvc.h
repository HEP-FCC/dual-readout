#ifndef GEOSVC_H
#define GEOSVC_H 1

// DD4Hep
#include "DD4hep/Detector.h"

// Geant4
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

class GeoSvc {

public:
  /// Default constructor
  GeoSvc(std::vector<std::string> names);

  /// Destructor
  virtual ~GeoSvc();
  /// Initialize function
  void initialize();
  /// This function generates the DD4hep geometry
  void buildDD4HepGeo();
  /// This function generates the Geant4 geometry
  void buildGeant4Geo();
  // receive DD4hep Geometry
  dd4hep::DetElement getDD4HepGeo();
  dd4hep::Detector* lcdd();
  // receive Geant4 Geometry
  G4VUserDetectorConstruction* getGeant4Geo();

  static GeoSvc* GetInstance();

private:
  /// Pointer to the interface to the DD4hep geometry
  dd4hep::Detector* m_dd4hepgeo;
  /// Pointer to the detector construction of DDG4
  std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo;
  /// XML-files with the detector description
  std::vector<std::string> m_xmlFileNames;

  static GeoSvc* fInstance;
};

#endif // GEOSVC_H
