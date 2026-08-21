// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/controller/DifferentialDriveWheelVoltages.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * A helper class which computes the feedforward outputs for a differential
 * drive drivetrain.
 */
class WPILIB_DLLEXPORT DifferentialDriveFeedforward {
  wpi::math::LinearSystem<2, 2, 2> m_plant;

 public:
  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param V_LINEAR The linear velocity gain in volts per (meters per second).
   * @param A_LINEAR The linear acceleration gain in volts per (meters per
   * second squared).
   * @param V_ANGULAR The angular velocity gain in volts per (radians per
   * second).
   * @param A_ANGULAR The angular acceleration gain in volts per (radians per
   * second squared).
   * @param trackwidth The distance between the differential drive's left and
   * right wheels, in meters.
   */
  constexpr DifferentialDriveFeedforward(
      decltype(1_V / 1_mps) V_LINEAR, decltype(1_V / 1_mps_sq) A_LINEAR,
      decltype(1_V / 1_rad_per_s) V_ANGULAR,
      decltype(1_V / 1_rad_per_s_sq) A_ANGULAR, wpi::units::meter_t trackwidth)
      // See Models::DifferentialDriveFromSysId(decltype(1_V / 1_mps),
      // decltype(1_V / 1_mps_sq), decltype(1_V / 1_rad_per_s), decltype(1_V /
      // 1_rad_per_s_sq))
      : DifferentialDriveFeedforward{V_LINEAR, A_LINEAR,
                                     V_ANGULAR * 2.0 / trackwidth * 1_rad,
                                     A_ANGULAR * 2.0 / trackwidth * 1_rad} {}

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param V_LINEAR The linear velocity gain in volts per (meters per second).
   * @param A_LINEAR The linear acceleration gain in volts per (meters per
   * second squared).
   * @param V_ANGULAR The angular velocity gain in volts per (meters per
   * second).
   * @param A_ANGULAR The angular acceleration gain in volts per (meters per
   * second squared).
   */
  constexpr DifferentialDriveFeedforward(decltype(1_V / 1_mps) V_LINEAR,
                                         decltype(1_V / 1_mps_sq) A_LINEAR,
                                         decltype(1_V / 1_mps) V_ANGULAR,
                                         decltype(1_V / 1_mps_sq) A_ANGULAR)
      : m_plant{wpi::math::Models::DifferentialDriveFromSysId(
            V_LINEAR, A_LINEAR, V_ANGULAR, A_ANGULAR)},
        V_LINEAR{V_LINEAR},
        A_LINEAR{A_LINEAR},
        V_ANGULAR{V_ANGULAR},
        A_ANGULAR{A_ANGULAR} {}

  /**
   * Calculates the differential drive feedforward inputs given velocity
   * references.
   *
   * @param currentLeftVelocity The current left velocity of the differential
   * drive in meters/second.
   * @param nextLeftVelocity The next left velocity of the differential drive in
   * meters/second.
   * @param currentRightVelocity The current right velocity of the differential
   * drive in meters/second.
   * @param nextRightVelocity The next right velocity of the differential drive
   * in meters/second.
   * @param dt Discretization timestep.
   */
  DifferentialDriveWheelVoltages Calculate(
      wpi::units::meters_per_second_t currentLeftVelocity,
      wpi::units::meters_per_second_t nextLeftVelocity,
      wpi::units::meters_per_second_t currentRightVelocity,
      wpi::units::meters_per_second_t nextRightVelocity,
      wpi::units::second_t dt);

  decltype(1_V / 1_mps) V_LINEAR;
  decltype(1_V / 1_mps_sq) A_LINEAR;
  decltype(1_V / 1_mps) V_ANGULAR;
  decltype(1_V / 1_mps_sq) A_ANGULAR;
};
}  // namespace wpi::math

#include "wpi/math/controller/proto/DifferentialDriveFeedforwardProto.hpp"
#include "wpi/math/controller/struct/DifferentialDriveFeedforwardStruct.hpp"
