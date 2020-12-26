// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

namespace frc {

/**
 * Interface for Controllers.
 *
 * Common interface for controllers. Controllers run control loops, the most
 * common are PID controllers and their variants, but this includes anything
 * that is controlling an actuator in a separate thread.
 *
 * @deprecated Only used by the deprecated PIDController
 */
class Controller {
 public:
  WPI_DEPRECATED("Only used by the deprecated PIDController")
  Controller() = default;
  virtual ~Controller() = default;

  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;

  /**
   * Allows the control loop to run
   */
  virtual void Enable() = 0;

  /**
   * Stops the control loop from running until explicitly re-enabled by calling
   * enable()
   */
  virtual void Disable() = 0;
};

}  // namespace frc
