// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/simulation/LinearSystemSim.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/moment_of_inertia.hpp"

namespace wpi::sim {
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
   *                           wpi::math::LinearSystemId::SingleJointedArmSystem().
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
  SingleJointedArmSim(const wpi::math::LinearSystem<2, 1, 2>& system,
                      const wpi::math::DCMotor& gearbox, double gearing,
                      wpi::units::meter_t armLength,
                      wpi::units::radian_t minAngle,
                      wpi::units::radian_t maxAngle, bool simulateGravity,
                      wpi::units::radian_t startingAngle,
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
  SingleJointedArmSim(
      const wpi::math::DCMotor& gearbox, double gearing,
      wpi::units::kilogram_square_meter_t moi, wpi::units::meter_t armLength,
      wpi::units::radian_t minAngle, wpi::units::radian_t maxAngle,
      bool simulateGravity, wpi::units::radian_t startingAngle,
      const std::array<double, 2>& measurementStdDevs = {0.0, 0.0});

  using LinearSystemSim::SetState;

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and
   * maximum allowed limits.
   *
   * @param angle The new angle.
   * @param velocity The new angular velocity.
   */
  void SetState(wpi::units::radian_t angle,
                wpi::units::radians_per_second_t velocity);

  /**
   * Returns whether the arm would hit the lower limit.
   *
   * @param armAngle The arm height.
   * @return Whether the arm would hit the lower limit.
   */
  bool WouldHitLowerLimit(wpi::units::radian_t armAngle) const;

  /**
   * Returns whether the arm would hit the upper limit.
   *
   * @param armAngle The arm height.
   * @return Whether the arm would hit the upper limit.
   */
  bool WouldHitUpperLimit(wpi::units::radian_t armAngle) const;

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
  wpi::units::radian_t GetAngle() const;

  /**
   * Returns the current arm velocity.
   *
   * @return The current arm velocity.
   */
  wpi::units::radians_per_second_t GetVelocity() const;

  /**
   * Returns the arm current draw.
   *
   * @return The arm current draw.
   */
  wpi::units::ampere_t GetCurrentDraw() const;

  /**
   * Sets the input voltage for the arm.
   *
   * @param voltage The input voltage.
   */
  void SetInputVoltage(wpi::units::volt_t voltage);

  /**
   * Calculates a rough estimate of the moment of inertia of an arm given its
   * length and mass.
   *
   * @param length The length of the arm.
   * @param mass   The mass of the arm.
   *
   * @return The calculated moment of inertia.
   */
  static constexpr wpi::units::kilogram_square_meter_t EstimateMOI(
      wpi::units::meter_t length, wpi::units::kilogram_t mass) {
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
  wpi::math::Vectord<2> UpdateX(const wpi::math::Vectord<2>& currentXhat,
                                const wpi::math::Vectord<1>& u,
                                wpi::units::second_t dt) override;

 private:
  wpi::units::meter_t m_armLen;
  wpi::units::radian_t m_minAngle;
  wpi::units::radian_t m_maxAngle;
  const wpi::math::DCMotor m_gearbox;
  double m_gearing;
  bool m_simulateGravity;
};
}  // namespace wpi::sim
