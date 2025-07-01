
#include "rpy/TelemetryTunable.h"
#include "semiwrap_init.wpilib._wpilib.hpp"

SEMIWRAP_PYBIND11_MODULE(m) {
  wpi::InitTelemetryTunable(m);
  initWrapper(m);
}
