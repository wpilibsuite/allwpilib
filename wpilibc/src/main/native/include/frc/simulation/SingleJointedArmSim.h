// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cmath>

#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/moment_of_inertia.h>
#include <wpi/SmallVector.h>

#include "frc/EigenCore.h"
#include "frc/simulation/AngularMechanismSim.h"
#include "frc/system/plant/DCMotor.h"

namespace frc::sim {
/**
 * Represents a simulated arm mechanism.
 */
class SingleJointedArmSim : public AngularMechanismSim {
 public:
  /**
   * Creates a simulated arm mechanism.
   *
   * @param system             The system representing this arm. This system can
   *                           be created with
   *                           LinearSystemId::SingleJointedArmSystem().
   * @param gearbox            The type and number of motors on the arm gearbox.
   * @param armLength          The length of the arm.
   * @param pivotPoint         The pivot point of the arm.
   * @param minAngle           The minimum angle that the arm is capable of.
   * @param maxAngle           The maximum angle that the arm is capable of.
   * @param simulateGravity    Whether gravity should be simulated or not.
   * @param startingAngle      The initial position of the arm.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  SingleJointedArmSim(const LinearSystem<2, 1, 2>& system,
                      const DCMotor& gearbox, units::meter_t armLength,
                      units::meter_t pivotPoint, units::radian_t minAngle,
                      units::radian_t maxAngle, bool simulateGravity,
                      units::radian_t startingAngle,
                      const std::array<double, 2>& measurementStdDevs = {0.0,
                                                                         0.0});

  /**
   * Returns the arm length of the system.
   *
   * @return the arm length.
   */
  units::meter_t GetArmLength() { return m_armLen; }

  /**
   * Returns the pivot point of the system.
   *
   * @return the pivot point.
   */
  units::meter_t GetPivotPoint() { return m_pivotPoint; }

  /**
   * Returns the minimum angle of the system.
   *
   * @return the minimum angle.
   */
  units::radian_t GetMinAngle() { return m_minAngle; }

  /**
   * Returns the minimum angle of the system.
   *
   * @return the minimum angle.
   */
  units::radian_t GetMaxAngle() { return m_maxAngle; }

  /**
   * Returns the mass of the system.
   *
   * @return the mass.
   */
  units::kilogram_t GetMass() {
    return units::kilogram_t{GetJ().value() /
                             ((1.0 / 12.0) * std::pow(m_armLen.value(), 2) +
                              std::pow(m_pivotPoint.value(), 2))};
  }

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and
   * maximum allowed limits.
   *
   * @param angle The new angle.
   * @param velocity The new angular velocity.
   */
  void SetState(units::radian_t angle, units::radians_per_second_t velocity);

  /**
   * Sets the arm's position.  The new angle will be limited between the minimum
   * and maximum allowed limits.
   *
   * @param angle The new angle.
   */
  void SetPosition(units::radian_t angle);

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
  units::meter_t m_pivotPoint;
  units::radian_t m_minAngle;
  units::radian_t m_maxAngle;
  const DCMotor m_gearbox;
  bool m_simulateGravity;
};
}  // namespace frc::sim
