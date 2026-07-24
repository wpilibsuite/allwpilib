#pragma once

#include <pybind11/pybind11.h>

namespace wpi {

void InitTelemetryPython(pybind11::module_& m);

}  // namespace wpi
