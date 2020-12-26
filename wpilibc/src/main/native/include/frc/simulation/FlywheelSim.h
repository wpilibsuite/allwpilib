// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angular_velocity.h>
#include <units/moment_of_inertia.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
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
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  FlywheelSim(const LinearSystem<1, 1, 1>& plant, const DCMotor& gearbox,
              double gearing,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Creates a simulated flywhel mechanism.
   *
   * @param gearbox            The type of and number of motors in the flywheel
   *                           gearbox.
   * @param gearing            The gearing of the flywheel (numbers greater than
   *                           1 represent reductions).
   * @param moi                The moment of inertia of the flywheel.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  FlywheelSim(const DCMotor& gearbox, double gearing,
              units::kilogram_square_meter_t moi,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Returns the flywheel velocity.
   *
   * @return The flywheel velocity.
   */
  units::radians_per_second_t GetAngularVelocity() const;

  /**
   * Returns the flywheel current draw.
   *
   * @return The flywheel current draw.
   */
  units::ampere_t GetCurrentDraw() const override;

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

 private:
  DCMotor m_gearbox;
  double m_gearing;
};
}  // namespace frc::sim
