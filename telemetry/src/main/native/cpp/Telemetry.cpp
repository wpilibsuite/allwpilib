// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/Telemetry.hpp"

#include "wpi/telemetry/TelemetryRegistry.hpp"

using namespace wpi;
using namespace wpi::telemetry;

wpi::telemetry::TelemetryTable& Telemetry::GetTable() {
  static wpi::telemetry::TelemetryTable& root =
      wpi::telemetry::TelemetryRegistry::GetTable("");
  return root;
}
