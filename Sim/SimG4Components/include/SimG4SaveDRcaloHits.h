#ifndef SimG4SaveDRcaloHits_h
#define SimG4SaveDRcaloHits_h 1

#include "GeoSvc.h"

#include "DRsimInterface.h"

/** @class SimG4SaveCalHits SimG4Components/src/SimG4SaveCalHits.h SimG4SaveCalHits.h
 *
 *  Save calorimeter hits tool.
 *  All collections passed in the job options will be saved (\b'readoutNames').
 *  Readout name is defined in DD4hep XML file as the attribute 'readout' of 'detector' tag.
 *  If (\b'readoutNames') contain no elements or names that do not correspond to any hit collection,
 *  tool will fail at initialization.
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */

class SimG4SaveDRcaloHits {
public:
  explicit SimG4SaveDRcaloHits();
  virtual ~SimG4SaveDRcaloHits();

  void initialize();

  void saveOutput(const G4Event* aEvent);

  void setEventData(DRsimInterface::DRsimEventData* evtData) { fEventData = evtData; }

private:
  /// Pointer to the geometry service
  GeoSvc* m_geoSvc;

  DRsimInterface::DRsimEventData* fEventData;

  /// Name of the readouts (hits collections) to save
  std::vector<std::string> m_readoutNames;
};

#endif
