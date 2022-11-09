// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>

#include "frc/EigenCore.h"
#include "frc/controller/DifferentialDriveWheelVoltages.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/time.h"
#include "units/voltage.h"

namespace frc {
/**
 * A helper class which computes the feedforward outputs for a differential
 * drive drivetrain.
 */
class DifferentialDriveFeedforward {
  frc::LinearSystem<2, 2, 2> m_plant;

 public:
  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
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
  DifferentialDriveFeedforward(double kVLinear, double kALinear,
                               double kVAngular, double kAAngular,
                               double trackwidth)
      : m_plant{frc::LinearSystemId::IdentifyDrivetrainSystem(
            units::volt_t{kVLinear} / 1_mps, units::volt_t{kALinear} / 1_mps_sq,
            units::volt_t{kVAngular} / 1_rad_per_s,
            units::volt_t{kAAngular} / 1_rad_per_s_sq,
            units::meter_t{trackwidth})} {}

  /**
   * Calculates the differential drive feedforward inputs given velocity
   * setpoints.
   * @param currentLVelocity The current left velocity of the differential drive
   * in meters/second.
   * @param nextLVelocity The next left velocity of the differential drive in
   * meters/second.
   * @param currentRVelocity The current right velocity of the differential
   * drive in meters/second.
   * @param nextRVelocity The next right velocity of the differential drive in
   * meters/second.
   * @param dtSeconds Discretization timestep.
   */
  DifferentialDriveWheelVoltages calculate(double currentLVelocity,
                                           double nextLVelocity,
                                           double currentRVelocity,
                                           double nextRVelocity,
                                           double dtSeconds);
};
}  // namespace frc
