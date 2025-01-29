// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace wpi {

/**
 * Top-level interface for opmode classes. Users should generally extend one of
 * the abstract implementations of this interface (e.g. PeriodicOpMode or
 * LinearOpMode) rather than directly implementing this interface.
 */
class OpMode {
 public:
  /**
   * The object is destroyed when the opmode is no longer selected on the DS or
   * after OpModeRun() returns.
   */
  virtual ~OpMode() = default;

  /**
   * This function is called periodically while the opmode is selected on the DS
   * (robot is disabled). Code that should only run once when the opmode is
   * selected should go in the opmode constructor.
   */
  virtual void DisabledPeriodic() {}

  /**
   * This function is called when the opmode starts (robot is enabled).
   *
   * @param opModeId opmode unique ID
   */
  virtual void OpModeRun(int64_t opModeId) = 0;

  /**
   * This function is called asynchronously when the robot is disabled, to
   * request the opmode return from OpModeRun().
   */
  virtual void OpModeStop() = 0;
};

}  // namespace wpi
