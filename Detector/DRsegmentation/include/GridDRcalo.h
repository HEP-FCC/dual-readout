#ifndef GridDRcalo_h
#define GridDRcalo_h 1

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
namespace DDSegmentation {
class GridDRcalo : public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridDRcalo(const std::string& aCellEncoding);
  /// Default constructor used by derived classes passing an existing decoder
  GridDRcalo(const BitFieldCoder* decoder);
  /// destructor
  virtual ~GridDRcalo() = default;

  /**  Determine the global position based on the cell ID.
   *   @warning This segmentation has no knowledge of radius, so radius = 1 is taken into calculations.
   *   @param[in] aCellId ID of a cell.
   *   return Position (radius = 1). #TODO modify description
   */
  virtual Vector3D position(const CellID& aCellID) const;
  /**  Determine the cell ID based on the position.
   *   @param[in] aLocalPosition (not used).
   *   @param[in] aGlobalPosition position in the global coordinates.
   *   @param[in] aVolumeId ID of a volume.
   *   return Cell ID. #TODO modify description
   */
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition,
                        const VolumeID& aVolumeID) const;

  VolumeID volumeID(int numEta, int numPhi) const;
  VolumeID volumeID(int numEta, int numPhi, int numX, int numY, int x, int y) const;

  // Get the identifier number of a mother tower in eta or phi direction
  int numEta(const CellID& aCellID) const;
  int numPhi(const CellID& aCellID) const;

  // Get the total number of SiPMs of the mother tower in x or y direction (local coordinate)
  int numX(const CellID& aCellID) const;
  int numY(const CellID& aCellID) const;

  // Get the identifier number of a SiPM in x or y direction (local coordinate)
  int x(const CellID& aCellID) const; // approx eta direction
  int y(const CellID& aCellID) const; // approx phi direction

  bool IsCerenkov(const CellID& aCellID) const;
  bool IsCerenkov(int col, int row) const;

  bool IsTower(const CellID& aCellID) const;
  bool IsSiPM(const CellID& aCellID) const;

  inline const std::string& fieldNameNumEta() const { return fNumEtaId; }
  inline const std::string& fieldNameNumPhi() const { return fNumPhiId; }
  inline const std::string& fieldNameNumX() const { return fNumXId; }
  inline const std::string& fieldNameNumY() const { return fNumYId; }
  inline const std::string& fieldNameX() const { return fXId; }
  inline const std::string& fieldNameY() const { return fYId; }
  inline const std::string& fieldNameIsCerenkov() const { return fIsCerenkovId; }
  inline const std::string& fieldNameModule() const { return fModule; }

  inline void setFieldNameNumEta(const std::string& fieldName) { fNumEtaId = fieldName; }
  inline void setFieldNameNumPhi(const std::string& fieldName) { fNumPhiId = fieldName; }
  inline void setFieldNameNumX(const std::string& fieldName) { fNumXId = fieldName; }
  inline void setFieldNameNumY(const std::string& fieldName) { fNumYId = fieldName; }
  inline void setFieldNameX(const std::string& fieldName) { fXId = fieldName; }
  inline void setFieldNameY(const std::string& fieldName) { fYId = fieldName; }
  inline void setFieldNameIsCerenkov(const std::string& fieldName) { fIsCerenkovId = fieldName; }
  inline void setFieldNameModule(const std::string& fieldName) { fModule = fieldName; }

protected:
  std::string fNumEtaId;
  std::string fNumPhiId;
  std::string fNumXId;
  std::string fNumYId;
  std::string fXId;
  std::string fYId;
  std::string fIsCerenkovId;
  std::string fModule;
};
}
}

#endif
