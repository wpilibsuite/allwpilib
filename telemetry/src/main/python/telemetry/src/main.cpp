
#include "TelemetryPython.h"
#include "semiwrap_init.telemetry._telemetry.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  initWrapper(m);
  wpi::InitTelemetryPython(m);
}
