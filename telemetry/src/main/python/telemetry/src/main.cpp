#include "rpy/MockTelemetryBackendFunctions.h"
#include "semiwrap_init.telemetry._telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
  wpi::telemetry::python::InitializeMockBackendValueTypes(m);

  static int unused;  // the capsule needs something to reference
  py::capsule cleanup(&unused, [](void*) {
    // Release Python callbacks, backends, and cached entries during module
    // teardown, not from C++ static destructors after Python has finalized.
    wpi::telemetry::TelemetryRegistry::SetReportWarning(nullptr);
    wpi::telemetry::TelemetryRegistry::Reset();
  });
  m.add_object("_cleanup", cleanup);
}
