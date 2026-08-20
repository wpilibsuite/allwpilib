// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/Telemetry.hpp"

#include "wpi/telemetry/TelemetryRegistry.hpp"

namespace wpi::telemetry {

TelemetryTable& GetTable() {
  static TelemetryTable& root = TelemetryRegistry::GetTable("");
  return root;
}

}  // namespace wpi::telemetry
