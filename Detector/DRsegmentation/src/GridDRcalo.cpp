#include "GridDRcalo.h"

#include <climits>
#include <cmath>

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridDRcalo::GridDRcalo(const std::string& cellEncoding) : Segmentation(cellEncoding) {
  // define type and description
  _type = "GridDRcalo";
  _description = "DRcalo segmentation based on the tower / (Cerenkov or Scintillation) fiber / SiPM hierarchy";

  // register all necessary parameters
  registerIdentifier("identifier_eta", "Cell ID identifier for numEta", fNumEtaId, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for numPhi", fNumPhiId, "phi");
  registerIdentifier("identifier_numx", "Cell ID identifier for numX", fNumXId, "xmax");
  registerIdentifier("identifier_numy", "Cell ID identifier for numY", fNumYId, "ymax");
  registerIdentifier("identifier_x", "Cell ID identifier for x", fXId, "x");
  registerIdentifier("identifier_y", "Cell ID identifier for y", fYId, "y");
  registerIdentifier("identifier_IsCerenkov", "Cell ID identifier for IsCerenkov", fIsCerenkovId, "c");
  registerIdentifier("identifier_module", "Cell ID identifier for module", fModule, "module");
}

GridDRcalo::GridDRcalo(const BitFieldCoder* decoder) : Segmentation(decoder) {
  // define type and description
  _type = "GridDRcalo";
  _description = "DRcalo segmentation based on the tower / (Cerenkov or Scintillation) fiber / SiPM hierarchy";

  // register all necessary parameters
  registerIdentifier("identifier_eta", "Cell ID identifier for numEta", fNumEtaId, "eta");
  registerIdentifier("identifier_phi", "Cell ID identifier for numPhi", fNumPhiId, "phi");
  registerIdentifier("identifier_numx", "Cell ID identifier for numX", fNumXId, "xmax");
  registerIdentifier("identifier_numy", "Cell ID identifier for numY", fNumYId, "ymax");
  registerIdentifier("identifier_x", "Cell ID identifier for x", fXId, "x");
  registerIdentifier("identifier_y", "Cell ID identifier for y", fYId, "y");
  registerIdentifier("identifier_IsCerenkov", "Cell ID identifier for IsCerenkov", fIsCerenkovId, "c");
  registerIdentifier("identifier_module", "Cell ID identifier for module", fModule, "module");
}

/// determine the local based on the cell ID
Vector3D GridDRcalo::position(const CellID& cID) const { // #TODO retrieve position by cell ID
  Vector3D cellPosition{0,0,0};
  return cellPosition;
}

/// determine the cell ID based on the position
CellID GridDRcalo::cellID(const Vector3D& localPosition, const Vector3D& /*globalPosition*/, const VolumeID& vID) const {
  int numx = numX(vID);
  int numy = numY(vID);

  auto localX = localPosition.x();
  auto localY = localPosition.y();

  int x = std::floor( ( localX + ( numx%2==0 ? 0. : fGridSize/2. ) ) / fGridSize ) + numx/2;
  int y = std::floor( ( localY + ( numy%2==0 ? 0. : fGridSize/2. ) ) / fGridSize ) + numy/2;

  return setCellID( numEta(vID), numPhi(vID), numx, numy, x, y );
}

VolumeID GridDRcalo::setVolumeID(int numEta, int numPhi) const {
  VolumeID numEtaId = static_cast<VolumeID>(numEta);
  VolumeID numPhiId = static_cast<VolumeID>(numPhi);
  VolumeID vID = 0;
  _decoder->set(vID, fNumEtaId, numEtaId);
  _decoder->set(vID, fNumPhiId, numPhiId);

  VolumeID module = 0; // Tower, SiPM layer attached to the tower, etc.
  _decoder->set(vID, fModule, module);

  return vID;
}

CellID GridDRcalo::setCellID(int numEta, int numPhi, int numX, int numY, int x, int y) const {
  VolumeID numEtaId = static_cast<VolumeID>(numEta);
  VolumeID numPhiId = static_cast<VolumeID>(numPhi);
  VolumeID numXId = static_cast<VolumeID>(numX);
  VolumeID numYId = static_cast<VolumeID>(numY);
  VolumeID xId = static_cast<VolumeID>(x);
  VolumeID yId = static_cast<VolumeID>(y);
  VolumeID vID = 0;
  _decoder->set(vID, fNumEtaId, numEtaId);
  _decoder->set(vID, fNumPhiId, numPhiId);
  _decoder->set(vID, fNumXId, numXId);
  _decoder->set(vID, fNumYId, numYId);
  _decoder->set(vID, fXId, xId);
  _decoder->set(vID, fYId, yId);

  VolumeID module = 1; // Fiber, SiPM, etc.
  _decoder->set(vID, fModule, module);

  VolumeID isCeren = IsCerenkov(x,y) ? 1 : 0;
  _decoder->set(vID, fIsCerenkovId, isCeren);

  return vID;
}

// Get the identifier number of a mother tower in eta or phi direction
int GridDRcalo::numEta(const CellID& aCellID) const {
  VolumeID numEta = static_cast<VolumeID>(_decoder->get(aCellID, fNumEtaId));
  return static_cast<int>(numEta);
}

int GridDRcalo::numPhi(const CellID& aCellID) const {
  VolumeID numPhi = static_cast<VolumeID>(_decoder->get(aCellID, fNumPhiId));
  return static_cast<int>(numPhi);
}

// Get the total number of SiPMs of the mother tower in x or y direction (local coordinate)
int GridDRcalo::numX(const CellID& aCellID) const {
  VolumeID numX = static_cast<VolumeID>(_decoder->get(aCellID, fNumXId));
  return static_cast<int>(numX);
}

int GridDRcalo::numY(const CellID& aCellID) const {
  VolumeID numY = static_cast<VolumeID>(_decoder->get(aCellID, fNumYId));
  return static_cast<int>(numY);
}

// Get the identifier number of a SiPM in x or y direction (local coordinate)
int GridDRcalo::x(const CellID& aCellID) const { // approx eta direction
  VolumeID x = static_cast<VolumeID>(_decoder->get(aCellID, fXId));
  return static_cast<int>(x);
}
int GridDRcalo::y(const CellID& aCellID) const { // approx phi direction
  VolumeID y = static_cast<VolumeID>(_decoder->get(aCellID, fYId));
  return static_cast<int>(y);
}

bool GridDRcalo::IsCerenkov(const CellID& aCellID) const {
  VolumeID isCeren = static_cast<VolumeID>(_decoder->get(aCellID, fIsCerenkovId));
  return static_cast<bool>(isCeren);
}

bool GridDRcalo::IsCerenkov(int col, int row) const {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}

bool GridDRcalo::IsTower(const CellID& aCellID) const {
  VolumeID module = static_cast<VolumeID>(_decoder->get(aCellID, fModule));
  return module==0;
}

bool GridDRcalo::IsSiPM(const CellID& aCellID) const {
  VolumeID module = static_cast<VolumeID>(_decoder->get(aCellID, fModule));
  return module==1;
}

int GridDRcalo::getLast32bits(const CellID& aCellID) const {
  CellID aId64 = aCellID >> sizeof(int)*CHAR_BIT;
  int aId32 = (int)aId64;

  return aId32;
}

CellID GridDRcalo::convertLast32to64(const int aId32) const {
  CellID aId64 = (CellID)aId32;
  aId64 <<= sizeof(int)*CHAR_BIT;

  return aId64;
}

REGISTER_SEGMENTATION(GridDRcalo)
}
}
