// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "Eigen/Core"
#include "frc/system/LinearSystem.h"
#include "units/acceleration.h"
#include "units/angular_acceleration.h"
#include "units/length.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {

/**
 * Filters the provided voltages to limit a differential drive's linear and
 * angular acceleration.
 *
 * The differential drive model can be created via the functions in
 * LinearSystemId.
 */
class WPILIB_DLLEXPORT DifferentialDriveAccelerationLimiter {
 public:
  /**
   * Motor voltages for a differential drive.
   */
  struct WheelVoltages {
    units::volt_t left = 0_V;
    units::volt_t right = 0_V;
  };

  /**
   * Constructs a DifferentialDriveAccelerationLimiter.
   *
   * @param system The differential drive dynamics.
   * @param trackwidth The trackwidth.
   * @param maxLinearAccel The maximum linear acceleration.
   * @param maxAngularAccel The maximum angular acceleration.
   */
  DifferentialDriveAccelerationLimiter(
      LinearSystem<2, 2, 2> system, units::meter_t trackwidth,
      units::meters_per_second_squared_t maxLinearAccel,
      units::radians_per_second_squared_t maxAngularAccel);

  /**
   * Returns the next voltage pair subject to acceleraiton constraints.
   *
   * @param leftVelocity The left wheel velocity.
   * @param rightVelocity The right wheel velocity.
   * @param leftVoltage The unconstrained left motor voltage.
   * @param rightVoltage The unconstrained right motor voltage.
   * @return The constrained wheel voltages.
   */
  WheelVoltages Calculate(units::meters_per_second_t leftVelocity,
                          units::meters_per_second_t rightVelocity,
                          units::volt_t leftVoltage,
                          units::volt_t rightVoltage);

 private:
  LinearSystem<2, 2, 2> m_system;
  units::meter_t m_trackwidth;
  units::meters_per_second_squared_t m_maxLinearAccel;
  units::radians_per_second_squared_t m_maxAngularAccel;
};

}  // namespace frc
