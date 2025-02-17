// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/SymbolExports.h"

namespace wpi {

class TelemetryTable;

/** The base interface for complex telemetry objects. */
class WPILIB_DLLEXPORT TelemetryLoggable {
  constexpr virtual ~TelemetryLoggable() = default;

  /**
   * Logs the object to a TelemetryTable.
   *
   * @param table telemetry table
   */
  virtual void Log(TelemetryTable& table) const = 0;
};

}  // namespace wpi
