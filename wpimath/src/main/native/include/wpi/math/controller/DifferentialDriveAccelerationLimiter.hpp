// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <utility>

#include <Eigen/Core>

#include "wpi/math/controller/DifferentialDriveWheelVoltages.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Filters the provided voltages to limit a differential drive's linear and
 * angular acceleration.
 *
 * The differential drive model can be created via the functions in Models.
 */
class WPILIB_DLLEXPORT DifferentialDriveAccelerationLimiter {
 public:
  /**
   * Constructs a DifferentialDriveAccelerationLimiter.
   *
   * @param system The differential drive dynamics.
   * @param trackwidth The distance between the differential drive's left and
   *                   right wheels.
   * @param maxLinearAccel The maximum linear acceleration.
   * @param maxAngularAccel The maximum angular acceleration.
   */
  DifferentialDriveAccelerationLimiter(
      LinearSystem<2, 2, 2> system, wpi::units::meters<> trackwidth,
      wpi::units::meters_per_second_squared<> maxLinearAccel,
      wpi::units::radians_per_second_squared<> maxAngularAccel)
      : DifferentialDriveAccelerationLimiter(system, trackwidth,
                                             -maxLinearAccel, maxLinearAccel,
                                             maxAngularAccel) {}

  /**
   * Constructs a DifferentialDriveAccelerationLimiter.
   *
   * @param system The differential drive dynamics.
   * @param trackwidth The distance between the differential drive's left and
   *                   right wheels.
   * @param minLinearAccel The minimum (most negative) linear acceleration.
   * @param maxLinearAccel The maximum (most positive) linear acceleration.
   * @param maxAngularAccel The maximum angular acceleration.
   * @throws std::invalid_argument if minimum linear acceleration is greater
   * than maximum linear acceleration
   */
  DifferentialDriveAccelerationLimiter(
      LinearSystem<2, 2, 2> system, wpi::units::meters<> trackwidth,
      wpi::units::meters_per_second_squared<> minLinearAccel,
      wpi::units::meters_per_second_squared<> maxLinearAccel,
      wpi::units::radians_per_second_squared<> maxAngularAccel)
      : m_system{std::move(system)},
        m_trackwidth{trackwidth},
        m_minLinearAccel{minLinearAccel},
        m_maxLinearAccel{maxLinearAccel},
        m_maxAngularAccel{maxAngularAccel} {
    if (minLinearAccel > maxLinearAccel) {
      throw std::invalid_argument(
          "maxLinearAccel must be greater than minLinearAccel");
    }
  }

  /**
   * Returns the next voltage pair subject to acceleration constraints.
   *
   * @param leftVelocity The left wheel velocity.
   * @param rightVelocity The right wheel velocity.
   * @param leftVoltage The unconstrained left motor voltage.
   * @param rightVoltage The unconstrained right motor voltage.
   * @return The constrained wheel voltages.
   */
  DifferentialDriveWheelVoltages Calculate(
      wpi::units::meters_per_second<> leftVelocity,
      wpi::units::meters_per_second<> rightVelocity,
      wpi::units::volts<> leftVoltage, wpi::units::volts<> rightVoltage);

 private:
  LinearSystem<2, 2, 2> m_system;
  wpi::units::meters<> m_trackwidth;
  wpi::units::meters_per_second_squared<> m_minLinearAccel;
  wpi::units::meters_per_second_squared<> m_maxLinearAccel;
  wpi::units::radians_per_second_squared<> m_maxAngularAccel;
};

}  // namespace wpi::math
