// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/controller/DifferentialDriveWheelVoltages.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/acceleration.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {
/**
 * A helper class which computes the feedforward outputs for a differential
 * drive drivetrain.
 */
class WPILIB_DLLEXPORT DifferentialDriveFeedforward {
  frc::LinearSystem<2, 2, 2> m_plant;

 public:
  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per
   * second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per
   * second).
   * @param kAAngular The angular acceleration gain in volts per (radians per
   * second squared).
   * @param trackwidth The distance between the differential drive's left and
   * right wheels, in meters.
   */
  constexpr DifferentialDriveFeedforward(
      decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
      decltype(1_V / 1_rad_per_s) kVAngular,
      decltype(1_V / 1_rad_per_s_sq) kAAngular, units::meter_t trackwidth)
      // See LinearSystemId::IdentifyDrivetrainSystem(decltype(1_V / 1_mps),
      // decltype(1_V / 1_mps_sq), decltype(1_V / 1_rad_per_s), decltype(1_V /
      // 1_rad_per_s_sq))
      : DifferentialDriveFeedforward{kVLinear, kALinear,
                                     kVAngular * 2.0 / trackwidth * 1_rad,
                                     kAAngular * 2.0 / trackwidth * 1_rad} {}

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per
   * second squared).
   * @param kVAngular The angular velocity gain in volts per (meters per
   * second).
   * @param kAAngular The angular acceleration gain in volts per (meters per
   * second squared).
   */
  constexpr DifferentialDriveFeedforward(decltype(1_V / 1_mps) kVLinear,
                                         decltype(1_V / 1_mps_sq) kALinear,
                                         decltype(1_V / 1_mps) kVAngular,
                                         decltype(1_V / 1_mps_sq) kAAngular)
      : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular)},
        m_kVLinear{kVLinear},
        m_kALinear{kALinear},
        m_kVAngular{kVAngular},
        m_kAAngular{kAAngular} {}

  /**
   * Calculates the differential drive feedforward inputs given velocity
   * setpoints.
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
      units::meters_per_second_t currentLeftVelocity,
      units::meters_per_second_t nextLeftVelocity,
      units::meters_per_second_t currentRightVelocity,
      units::meters_per_second_t nextRightVelocity, units::second_t dt);

  decltype(1_V / 1_mps) m_kVLinear;
  decltype(1_V / 1_mps_sq) m_kALinear;
  decltype(1_V / 1_mps) m_kVAngular;
  decltype(1_V / 1_mps_sq) m_kAAngular;
};
}  // namespace frc

#include "frc/controller/proto/DifferentialDriveFeedforwardProto.h"
#include "frc/controller/struct/DifferentialDriveFeedforwardStruct.h"
