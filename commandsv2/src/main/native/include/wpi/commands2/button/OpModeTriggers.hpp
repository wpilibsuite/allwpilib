// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/commands2/button/Trigger.hpp"

namespace wpi::cmd {

/**
 * Command-aware opmode descriptor used by CommandOpModes. Use the
 * Create*OpMode() methods to create command-aware opmode descriptors.
 *
 * An OpModeTriggers exposes triggers for selection and active/enabled state.
 */
class OpModeTriggers {
 public:
  /**
   * Constructs a command opmode descriptor.
   *
   * @param name OpMode name used for Driver Station selection matching.
   */
  explicit OpModeTriggers(std::string_view name);

  /**
   * Trigger that is true when this opmode is currently loaded on the Driver
   * Station.
   *
   * @return Loaded trigger.
   */
  Trigger Loaded() const;

  /**
   * Trigger that is true when this opmode is loaded and the robot is enabled.
   *
   * @return Active/enabled trigger.
   */
  Trigger Enabled() const;

  /**
   * Creates a trigger requiring this opmode to be loaded and the robot enabled
   * plus the provided trigger condition.
   *
   * @param other Additional trigger condition.
   * @return Combined trigger.
   */
  Trigger Enabled(Trigger other) const;

  /**
   * Trigger that is true when this opmode is loaded and the robot is disabled.
   *
   * @return Disabled trigger.
   */
  Trigger Disabled() const;

  /**
   * Creates a trigger requiring this opmode to be loaded and the robot disabled
   * plus the provided trigger condition.
   *
   * @param other Additional trigger condition.
   * @return Combined trigger.
   */
  Trigger Disabled(Trigger other) const;

 private:
  std::string m_name;
  Trigger m_loaded;
  Trigger m_enabled;
  Trigger m_disabled;
};

}  // namespace wpi::cmd
