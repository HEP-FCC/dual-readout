#include "DD4hep/Factories.h"
#include "DD4hep/detail/SegmentationsInterna.h"

namespace {
template <typename T>
dd4hep::SegmentationObject* create_segmentation(const dd4hep::BitFieldCoder* decoder) {
  return new dd4hep::SegmentationWrapper<T>(decoder);
}
} // namespace

#include "GridDRcalo.h"
DECLARE_SEGMENTATION(GridDRcalo, create_segmentation<dd4hep::DDSegmentation::GridDRcalo>)
