// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pybind11/pybind11.h"

namespace wpi {

/** Initializes the Python-friendly Telemetry and Tunable bindings. */
void InitTelemetryTunable(pybind11::module_& m);

}  // namespace wpi
