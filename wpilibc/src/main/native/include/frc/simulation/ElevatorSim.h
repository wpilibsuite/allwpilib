// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <units/length.h>
#include <units/mass.h>
#include <units/velocity.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated elevator mechanism.
 */
class ElevatorSim : public LinearSystemSim<2, 1, 1> {
 public:
  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param plant              The linear system that represents the elevator.
   * @param gearbox            The type of and number of motors in your
   *                           elevator gearbox.
   * @param gearing            The gearing of the elevator (numbers greater
   *                           than 1 represent reductions).
   * @param drumRadius         The radius of the drum that your cable is
   *                           wrapped around.
   * @param minHeight          The minimum allowed height of the elevator.
   * @param maxHeight          The maximum allowed height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   */
  ElevatorSim(const LinearSystem<2, 1, 1>& plant, const DCMotor& gearbox,
              double gearing, units::meter_t drumRadius,
              units::meter_t minHeight, units::meter_t maxHeight,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Constructs a simulated elevator mechanism.
   *
   * @param gearbox            The type of and number of motors in your
   *                           elevator gearbox.
   * @param gearing            The gearing of the elevator (numbers greater
   *                           than 1 represent reductions).
   * @param carriageMass       The mass of the elevator carriage.
   * @param drumRadius         The radius of the drum that your cable is
   *                           wrapped around.
   * @param minHeight          The minimum allowed height of the elevator.
   * @param maxHeight          The maximum allowed height of the elevator.
   * @param measurementStdDevs The standard deviation of the measurements.
   */
  ElevatorSim(const DCMotor& gearbox, double gearing,
              units::kilogram_t carriageMass, units::meter_t drumRadius,
              units::meter_t minHeight, units::meter_t maxHeight,
              const std::array<double, 1>& measurementStdDevs = {0.0});

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  bool WouldHitLowerLimit(units::meter_t elevatorHeight) const;

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  bool WouldHitUpperLimit(units::meter_t elevatorHeight) const;

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  bool HasHitLowerLimit() const;

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  bool HasHitUpperLimit() const;

  /**
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
   */
  units::meter_t GetPosition() const;

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
   */
  units::meters_per_second_t GetVelocity() const;

  /**
   * Returns the elevator current draw.
   *
   * @return The elevator current draw.
   */
  units::ampere_t GetCurrentDraw() const override;

  /**
   * Sets the input voltage for the elevator.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

 protected:
  /**
   * Updates the state estimate of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u           The system inputs (voltage).
   * @param dt          The time difference between controller updates.
   */
  Eigen::Matrix<double, 2, 1> UpdateX(
      const Eigen::Matrix<double, 2, 1>& currentXhat,
      const Eigen::Matrix<double, 1, 1>& u, units::second_t dt) override;

 private:
  DCMotor m_gearbox;
  units::meter_t m_drumRadius;
  units::meter_t m_minHeight;
  units::meter_t m_maxHeight;
  double m_gearing;
};
}  // namespace frc::sim
