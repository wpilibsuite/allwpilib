// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>

#include "frc/EigenCore.h"
#include "frc/controller/DifferentialDriveWheelVoltages.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
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
  DifferentialDriveFeedforward(decltype(1_V / 1_mps) kVLinear,
                               decltype(1_V / 1_mps_sq) kALinear,
                               decltype(1_V / 1_rad_per_s) kVAngular,
                               decltype(1_V / 1_rad_per_s_sq) kAAngular,
                               units::meter_t trackwidth)
      : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth)} {}

  /**
   * Calculates the differential drive feedforward inputs given velocity
   * setpoints.
   *
   * @param currentLVelocity The current left velocity of the differential drive
   * in meters/second.
   * @param nextLVelocity The next left velocity of the differential drive in
   * meters/second.
   * @param currentRVelocity The current right velocity of the differential
   * drive in meters/second.
   * @param nextRVelocity The next right velocity of the differential drive in
   * meters/second.
   * @param dt Discretization timestep.
   */
  DifferentialDriveWheelVoltages Calculate(
      units::meters_per_second_t currentLVelocity,
      units::meters_per_second_t nextLVelocity,
      units::meters_per_second_t currentRVelocity,
      units::meters_per_second_t nextRVelocity, units::second_t dt);
};
}  // namespace frc
