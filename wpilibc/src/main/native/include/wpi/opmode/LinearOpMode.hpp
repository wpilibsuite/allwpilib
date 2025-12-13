// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>

#include "wpi/opmode/OpMode.hpp"

namespace wpi {

/**
 * An opmode structure for "linear" operation. The user is responsible for
 * implementing any looping behavior; after Run() returns it will not be called
 * again on the same object.
 *
 * Lifecycle:
 *
 * - Constructed when opmode selected on driver station
 *
 * - DisabledPeriodic() called periodically as long as DS is disabled
 *
 * - When DS transitions from disabled to enabled, Run() is called exactly once
 *
 * - When DS transitions from enabled to disabled, or a different opmode is
 *   selected on the driver station, object is destroyed and not reused
 *
 * The user is responsible for exiting Run() when the opmode is directed to stop
 * executing. This is indicated by IsRunning() returning false. All other
 * methods should be written to return as quickly as possible when IsRunning()
 * returns false.
 */
class LinearOpMode : public OpMode {
 public:
  /**
   * Called periodically while the opmode is selected on the DS and the robot is
   * disabled.
   */
  void DisabledPeriodic() override {}

  /**
   * Called once when the robot is enabled. When it returns, it will not be
   * called again on the same object.
   */
  virtual void Run() = 0;

  /**
   * Returns true while this opmode is selected (regardless of enable state).
   * All other functions should be written to return as quickly as possible when
   * this returns false.
   *
   * @return True if opmode selected, false otherwise
   */
  bool IsRunning() const { return m_running; }

  // implements OpMode interface
  void OpModeRun(int64_t opModeId) final;

  void OpModeStop() final;

 private:
  std::atomic_bool m_running{true};
};

}  // namespace wpi
