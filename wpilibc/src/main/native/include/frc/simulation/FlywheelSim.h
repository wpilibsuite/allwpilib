/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/angular_velocity.h>
#include <units/moment_of_inertia.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc {
namespace sim {
/**
 * Represents a simulated flywheel mechanism.
 */
class FlywheelSim : public LinearSystemSim<1, 1, 1> {
 public:
  /**
   * Creates a simulated flywhel mechanism.
   *
   * @param plant              The linear system representing the flywheel.
   * @param gearbox            The type of and number of motors in the flywheel
   *                           gearbox.
   * @param gearing            The gearing of the flywheel (numbers greater than
   *                           1 represent reductions).
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  FlywheelSim(const LinearSystem<1, 1, 1>& plant, const DCMotor& gearbox,
              double gearing, bool addNoise = false,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Creates a simulated flywhel mechanism.
   *
   * @param gearbox            The type of and number of motors in the flywheel
   *                           gearbox.
   * @param gearing            The gearing of the flywheel (numbers greater than
   *                           1 represent reductions).
   * @param moi                The moment of inertia of the flywheel.
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  FlywheelSim(const DCMotor& gearbox, double gearing,
              units::kilogram_square_meter_t moi, bool addNoise = false,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Returns the flywheel velocity.
   * @return The flywheel velocity.
   */
  units::radians_per_second_t GetVelocity() const;

  /**
   * Returns the flywheel current draw.
   * @return The flywheel current draw.
   */
  units::ampere_t GetCurrentDraw() const override;

 private:
  DCMotor m_motor;
  double m_gearing;
};
}  // namespace sim
}  // namespace frc
