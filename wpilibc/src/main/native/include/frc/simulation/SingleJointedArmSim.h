/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
class SingleJointedArmSim : public LinearSystemSim<2, 1, 1> {
 public:
  /**
   * Creates a simulated arm mechanism.
   *
   * @param system             The system representing this arm.
   * @param motor              The type and number of motors on the arm gearbox.
   * @param G                  The gear ratio of the arm (numbers greater than 1
   *                           represent reductions).
   * @param mass               The mass of the arm.
   * @param armLength          The length of the arm.
   * @param minAngle           The minimum allowed angle for the arm.
   * @param maxAngle           The maximum allowed angle for the arm.
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   * @param simulateGravity    If the affects of gravity should be simulated.
   */
  SingleJointedArmSim(const LinearSystem<2, 1, 1>& system, const DCMotor motor,
                      double G, units::kilogram_t mass,
                      units::meter_t armLength, units::radian_t minAngle,
                      units::radian_t maxAngle, bool addNoise,
                      const std::array<double, 1>& measurementStdDevs,
                      bool simulateGravity);
  /**
   * Creates a simulated arm mechanism.
   *
   * @param motor              The type and number of motors on the arm gearbox.
   * @param J                  The moment of inertia of the arm.
   * @param G                  The gear ratio of the arm (numbers greater than 1
   *                           represent reductions).
   * @param mass               The mass of the arm.
   * @param armLength          The length of the arm.
   * @param minAngle           The minimum allowed angle for the arm.
   * @param maxAngle           The maximum allowed angle for the arm.
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   * @param simulateGravity    If the affects of gravity should be simulated.
   */
  SingleJointedArmSim(const DCMotor& motor, units::kilogram_square_meter_t J,
                      double G, units::kilogram_t mass,
                      units::meter_t armLength, units::radian_t minAngle,
                      units::radian_t maxAngle, bool addNoise,
                      const std::array<double, 1>& measurementStdDevs,
                      bool simulateGravity);

  /**
   * Creates a simulated arm mechanism.
   *
   * @param motor              The type and number of motors on the arm gearbox.
   * @param G                  The gear ratio of the arm (numbers greater than 1
   *                           represent reductions).
   * @param mass               The mass of the arm.
   * @param armLength          The length of the arm.
   * @param minAngle           The minimum allowed angle for the arm. This is
   * measured from horizontal, with straight out being 0.
   * @param maxAngle           The maximum allowed angle for the arm. This is
   * measured from horizontal, with straight out being 0.
   * @param addNoise           Whether the sim should automatically add some
   *                           encoder noise.
   * @param measurementStdDevs The standard deviation of the measurement noise.
   * @param simulateGravity    If the affects of gravity should be simulated.
   */
  SingleJointedArmSim(const DCMotor& motor, double G, units::kilogram_t mass,
                      units::meter_t armLength, units::radian_t minAngle,
                      units::radian_t maxAngle, bool addNoise,
                      const std::array<double, 1>& measurementStdDevs,
                      bool simulateGravity);

  /**
   * Returns whether the arm has hit the lower limit.
   *
   * @param x The current arm state.
   * @return Whether the arm has hit the lower limit.
   */
  bool HasHitLowerLimit(const Eigen::Matrix<double, 2, 1>& x) const;

  /**
   * Returns whether the arm has hit the upper limit.
   *
   * @param x The current arm state.
   * @return Whether the arm has hit the upper limit.
   */
  bool HasHitUpperLimit(const Eigen::Matrix<double, 2, 1>& x) const;

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

  units::ampere_t GetCurrentDraw() const override;

 protected:
  /**
   * Updates the state estimate of the arm.
   *
   * @param currentXhat The current state estimate.
   * @param u           The system inputs (voltage).
   * @param dt          The time difference between controller updates.
   */
  Eigen::Matrix<double, 2, 1> UpdateX(
      const Eigen::Matrix<double, 2, 1>& currentXhat,
      const Eigen::Matrix<double, 1, 1>& u, units::second_t dt) override;

 private:
  units::meter_t m_r;
  units::radian_t m_minAngle;
  units::radian_t m_maxAngle;
  units::kilogram_t m_mass;
  const DCMotor m_motor;
  double m_gearing;
  bool m_simulateGravity;
};
}  // namespace frc::sim
