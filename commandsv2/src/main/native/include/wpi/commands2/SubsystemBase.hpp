// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/commands2/Subsystem.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"

namespace wpi::cmd {
/**
 * A base for subsystems that handles registration in the constructor, and
 * provides a more intuitive method for setting the default command.
 *
 * This class is provided by the NewCommands VendorDep
 */
class SubsystemBase : public Subsystem, public wpi::TelemetryLoggable {
 public:
  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

  /**
   * Gets the name of this Subsystem.
   *
   * @return Name
   */
  std::string GetName() const override;

  /**
   * Sets the name of this Subsystem.
   *
   * @param name name
   */
  void SetName(std::string_view name);

 protected:
  /**
   * Constructor.  Telemetry/log name defaults to the classname.
   */
  SubsystemBase();
  /**
   * Constructor.
   *
   * @param name Name of the subsystem for telemetry and logging.
   */
  explicit SubsystemBase(std::string_view name);

 private:
  std::string m_name;
};
}  // namespace wpi::cmd
