#include "GeoSvc.h"
#include "GeoConstruction.h"
#include "TGeoManager.h"

#include <stdexcept>

#include "DD4hep/Printout.h"

GeoSvc* GeoSvc::fInstance = 0;

GeoSvc::GeoSvc(std::vector<std::string> names)
: m_dd4hepgeo(0), m_geant4geo(0), m_xmlFileNames(names) {
  initialize();

  if (fInstance==0) {
    fInstance = this;
  }
}

GeoSvc::~GeoSvc() {
  if (m_dd4hepgeo){
    try {
      m_dd4hepgeo->destroyInstance();
      m_dd4hepgeo = 0;
    } catch(...) {}
  }
}

void GeoSvc::initialize() {
  buildDD4HepGeo();
  buildGeant4Geo();

  return;
}

GeoSvc* GeoSvc::GetInstance() {
  return fInstance;
}

void GeoSvc::buildDD4HepGeo() {
  // we retrieve the the static instance of the DD4HEP::Geometry
  m_dd4hepgeo = &(dd4hep::Detector::getInstance());

  // load geometry
  if (m_xmlFileNames.size()==0) throw std::runtime_error("List of xml file names is empty!");

  for (auto& filename : m_xmlFileNames) {
    std::cout << "loading geometry from file:  '" << filename << "'" << std::endl;
    m_dd4hepgeo->fromCompact(filename);
  }
  m_dd4hepgeo->volumeManager();
  m_dd4hepgeo->apply("DD4hepVolumeManager", 0, 0);

  return;
}

dd4hep::Detector* GeoSvc::lcdd() { return (m_dd4hepgeo); }

dd4hep::DetElement GeoSvc::getDD4HepGeo() { return (lcdd()->world()); }

void GeoSvc::buildGeant4Geo() {
  std::shared_ptr<G4VUserDetectorConstruction> detector(new det::GeoConstruction(*lcdd()));
  m_geant4geo = detector;

  if (m_geant4geo) return;
  else throw std::runtime_error("Failed to build Geant4Geo");
}

G4VUserDetectorConstruction* GeoSvc::getGeant4Geo() { return (m_geant4geo.get()); }
