
#include "SmartDashboardData.h"

namespace rpy {

//
// Ensures that python objects added to the SmartDashboard have at least one
// reference to them
//
// All functions here must be called with the GIL held
//

static py::dict &getSmartDashboardData() {
  static py::dict data;
  return data;
}

void addSmartDashboardData(py::str &key, std::shared_ptr<wpi::Sendable> data) {
  auto &sdData = getSmartDashboardData();
  sdData[key] = py::cast(data);
}

void clearSmartDashboardData() {
  auto &sdData = getSmartDashboardData();
  if (sdData) {
    sdData.clear();
  }
}

void destroySmartDashboardData() {
  auto &sdData = getSmartDashboardData();
  if (sdData) {
    sdData.clear();
    // force the dictionary to be deleted otherwise it'll crash when libc++
    // is unwinding static objects after interpreter destruction
    sdData.dec_ref();
    // release our reference to this otherwise the destructor will try to
    // delete a non-existant PyObject* after interpreter destruction
    sdData.release();
  }
}

} // namespace rpy
