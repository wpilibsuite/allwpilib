// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/util/SymbolExports.hpp"

namespace wpi {

class TelemetryTable;

/**
 * The base interface for complex telemetry objects.
 */
class WPILIB_DLLEXPORT TelemetryLoggable {
  void anchor();  // provide a place for the vtable

 public:
  virtual ~TelemetryLoggable() = default;

  /**
   * Logs the object to a {@link TelemetryTable}.
   *
   * @param table telemetry table
   */
  virtual void UpdateTelemetry(TelemetryTable& table) const = 0;

  /**
   * Gets the telemetry table type. Default is no specified table type (empty).
   * A mismatch in type during logging results in a reported warning, so this
   * should always return the same value.
   *
   * @return Table type
   */
  virtual std::string_view GetTelemetryType() const { return {}; }
};

}  // namespace wpi
