/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
   * @param gearbox            The type of and number of motors in your elevator
   *                           gearbox.
   * @param carriageMass       The mass of the elevator carriage.
   * @param gearing            The gearing of the elevator (numbers greater than
   *                           1 represent reductions).
   * @param drumRadius         The radius of the drum that your cable is wrapped
   *                           around.
   * @param minHeight          The minimum allowed height of the elevator.
   * @param maxHeight          The maximum allowed height of the elevator.
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  ElevatorSim(const DCMotor& gearbox, units::kilogram_t carriageMass,
              double gearing, units::meter_t drumRadius,
              units::meter_t minHeight, units::meter_t maxHeight,
              bool addNoise = false,
              const std::array<double, 1>& m_measurementStdDevs = {0.0});

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @param x The current elevator state.
   * @return Whether the elevator has hit the lower limit.
   */
  bool HasHitLowerLimit(const Eigen::Matrix<double, 2, 1>& x) const;

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @param x The current elevator state.
   * @return Whether the elevator has hit the uppwer limit.
   */
  bool HasHitUpperLimit(const Eigen::Matrix<double, 2, 1>& x) const;

  /**
   * Returns the position of the elevator.
   * 
   * @return The position of the elevator.
   */
  units::meter_t GetElevatorPosition() const;

  /**
   * Returns the velocity of the elevator.
   * 
   * @return The velocity of the elevator.
   */
  units::meters_per_second_t GetElevatorVelocity() const;

  /**
   * Returns the elevator current draw.
   * 
   * @return The elevator current draw.
   */
  units::ampere_t GetCurrentDraw() const override;

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
  DCMotor m_motor;
  units::meter_t m_drumRadius;
  units::meter_t m_minHeight;
  units::meter_t m_maxHeight;
  double m_gearing;
};
}  // namespace sim
}  // namespace frc
