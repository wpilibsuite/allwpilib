// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/Telemetry.h"

#include "wpi/telemetry/TelemetryRegistry.h"

using namespace wpi;

wpi::TelemetryTable& Telemetry::GetTable() {
  static wpi::TelemetryTable& root = wpi::TelemetryRegistry::GetTable("");
  return root;
}
