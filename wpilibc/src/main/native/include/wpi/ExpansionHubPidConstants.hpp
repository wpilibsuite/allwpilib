// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/IntegerTopic.h>

namespace frc {
class ExpansionHubMotor;

/** This class contains PID constants for an ExpansionHub motor. */
class ExpansionHubPidConstants {
 public:
  /**
   * Sets the PID Controller gain parameters.
   *
   * Sets the proportional, integral, and differential coefficients.
   *
   * @param p The proportional coefficient. Must be >= 0.
   * @param i The integral coefficient. Must be >= 0.
   * @param d The differential coefficient. Must be >= 0.
   */
  void SetPID(double p, double i, double d);

  /**
   * Sets the feed forward gains to the specified values.
   *
   * The units should be radians for angular systems and meters for linear
   * systems.
   *
   * The PID control period is 10ms
   *
   * @param s The static gain in volts.
   * @param v The velocity gain in V/(units/s).
   * @param a The acceleration gain in V/(units/s²).
   */
  void SetFF(double s, double v, double a);

  /**
   * Enables continuous input.
   *
   * Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  void EnableContinousInput(double minimumInput, double maximumInput);

  /**
   * Disables continuous input.
   */
  void DisableContinousInput();

  ExpansionHubPidConstants(ExpansionHubPidConstants&) = delete;
  ExpansionHubPidConstants& operator=(ExpansionHubPidConstants&) = delete;

  ExpansionHubPidConstants(ExpansionHubPidConstants&&) = default;
  ExpansionHubPidConstants& operator=(ExpansionHubPidConstants&&) = default;

  friend class ExpansionHubMotor;

 private:
  ExpansionHubPidConstants(int usbId, int channel, bool isVelocityPid);

  nt::DoublePublisher m_pPublisher;
  nt::DoublePublisher m_iPublisher;
  nt::DoublePublisher m_dPublisher;
  nt::DoublePublisher m_sPublisher;
  nt::DoublePublisher m_vPublisher;
  nt::DoublePublisher m_aPublisher;

  nt::BooleanPublisher m_continuousPublisher;
  nt::DoublePublisher m_continuousMinimumPublisher;
  nt::DoublePublisher m_continuousMaximumPublisher;
};
}  // namespace frc
