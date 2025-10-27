// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/moment_of_inertia.h>

#include "frc/simulation/LinearSystemSim.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated arm mechanism.
 */
class SingleJointedArmSim : public LinearSystemSim<2, 1, 2> {
 public:
  /**
   * Creates a simulated arm mechanism.
   *
   * @param system             The system representing this arm. This system can
   *                           be created with
   *                           LinearSystemId::SingleJointedArmSystem().
   * @param gearbox            The type and number of motors on the arm gearbox.
   * @param gearing            The gear ratio of the arm (numbers greater than 1
   *                           represent reductions).
   * @param armLength          The length of the arm.
   * @param minAngle           The minimum angle that the arm is capable of.
   * @param maxAngle           The maximum angle that the arm is capable of.
   * @param simulateGravity    Whether gravity should be simulated or not.
   * @param startingAngle      The initial position of the arm.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  SingleJointedArmSim(const LinearSystem<2, 1, 2>& system,
                      const DCMotor& gearbox, double gearing,
                      units::meter_t armLength, units::radian_t minAngle,
                      units::radian_t maxAngle, bool simulateGravity,
                      units::radian_t startingAngle,
                      const std::array<double, 2>& measurementStdDevs = {0.0,
                                                                         0.0});
  /**
   * Creates a simulated arm mechanism.
   *
   * @param gearbox            The type and number of motors on the arm gearbox.
   * @param gearing            The gear ratio of the arm (numbers greater than 1
   *                           represent reductions).
   * @param moi                The moment of inertia of the arm. This can be
   *                           calculated from CAD software.
   * @param armLength          The length of the arm.
   * @param minAngle           The minimum angle that the arm is capable of.
   * @param maxAngle           The maximum angle that the arm is capable of.
   * @param simulateGravity    Whether gravity should be simulated or not.
   * @param startingAngle      The initial position of the arm.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   */
  SingleJointedArmSim(const DCMotor& gearbox, double gearing,
                      units::kilogram_square_meter_t moi,
                      units::meter_t armLength, units::radian_t minAngle,
                      units::radian_t maxAngle, bool simulateGravity,
                      units::radian_t startingAngle,
                      const std::array<double, 2>& measurementStdDevs = {0.0,
                                                                         0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and
   * maximum allowed limits.
   *
   * @param angle The new angle.
   * @param velocity The new angular velocity.
   */
  void SetState(units::radian_t angle, units::radians_per_second_t velocity);

  /**
   * Returns whether the arm would hit the lower limit.
   *
   * @param armAngle The arm height.
   * @return Whether the arm would hit the lower limit.
   */
  bool WouldHitLowerLimit(units::radian_t armAngle) const;

  /**
   * Returns whether the arm would hit the upper limit.
   *
   * @param armAngle The arm height.
   * @return Whether the arm would hit the upper limit.
   */
  bool WouldHitUpperLimit(units::radian_t armAngle) const;

  /**
   * Returns whether the arm has hit the lower limit.
   *
   * @return Whether the arm has hit the lower limit.
   */
  bool HasHitLowerLimit() const;

  /**
   * Returns whether the arm has hit the upper limit.
   *
   * @return Whether the arm has hit the upper limit.
   */
  bool HasHitUpperLimit() const;

  /**
   * Returns the current arm angle.
   *
   * @return The current arm angle.
   */
  units::radian_t GetAngle() const;

  /**
   * Returns the current arm velocity.
   *
   * @return The current arm velocity.
   */
  units::radians_per_second_t GetVelocity() const;

  /**
   * Returns the arm current draw.
   *
   * @return The arm current draw.
   */
  units::ampere_t GetCurrentDraw() const;

  /**
   * Sets the input voltage for the arm.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(units::volt_t voltage);

  /**
   * Calculates a rough estimate of the moment of inertia of an arm given its
   * length and mass.
   *
   * @param length The length of the arm.
   * @param mass   The mass of the arm.
   *
   * @return The calculated moment of inertia.
   */
  static constexpr units::kilogram_square_meter_t EstimateMOI(
      units::meter_t length, units::kilogram_t mass) {
    return 1.0 / 3.0 * mass * length * length;
  }

 protected:
  /**
   * Updates the state estimate of the arm.
   *
   * @param currentXhat The current state estimate.
   * @param u           The system inputs (voltage).
   * @param dt          The time difference between controller updates.
   */
  Vectord<2> UpdateX(const Vectord<2>& currentXhat, const Vectord<1>& u,
                     units::second_t dt) override;

 private:
  units::meter_t m_armLen;
  units::radian_t m_minAngle;
  units::radian_t m_maxAngle;
  const DCMotor m_gearbox;
  double m_gearing;
  bool m_simulateGravity;
};
}  // namespace frc::sim
