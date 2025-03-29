// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

class TelemetryTable;

class TelemetryLoggable {
 public:
  virtual ~TelemetryLoggable() = default;

  virtual void UpdateTelemetry(TelemetryTable& table) const = 0;
};

}  // namespace wpi
