// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/nt/DoubleTopic.hpp"

namespace wpi {
class ExpansionHubMotor;

/** This class contains feedback and feedforward constants for an ExpansionHub
 * motor. */
class ExpansionHubVelocityConstants {
 public:
  /**
   * Sets the PID Controller gain parameters.
   *
   * Set the proportional, integral, and differential coefficients.
   *
   * @param p The proportional coefficient.
   * @param i The integral coefficient.
   * @param d The derivative coefficient.
   * @return This object, for method chaining.
   */
  ExpansionHubVelocityConstants& SetPID(double p, double i, double d);

  /**
   * Sets the feed forward gains to the specified values.
   *
   * The units should be radians for angular systems and meters for linear
   * systems.
   *
   * The motor control period is 10ms
   *
   * @param s The static gain in volts.
   * @param v The velocity gain in volts per unit per second.
   * @param a The acceleration gain in volts per unit per second squared.
   * @return This object, for method chaining.
   */
  ExpansionHubVelocityConstants& SetFF(double s, double v, double a);

  ExpansionHubVelocityConstants(ExpansionHubVelocityConstants&) = delete;
  ExpansionHubVelocityConstants& operator=(ExpansionHubVelocityConstants&) =
      delete;

  ExpansionHubVelocityConstants(ExpansionHubVelocityConstants&&) = default;
  ExpansionHubVelocityConstants& operator=(ExpansionHubVelocityConstants&&) =
      default;

  friend class ExpansionHubMotor;

 private:
  ExpansionHubVelocityConstants(int hubNumber, int motorNumber);

  wpi::nt::DoublePublisher m_pPublisher;
  wpi::nt::DoublePublisher m_iPublisher;
  wpi::nt::DoublePublisher m_dPublisher;

  wpi::nt::DoublePublisher m_sPublisher;
  wpi::nt::DoublePublisher m_vPublisher;
  wpi::nt::DoublePublisher m_aPublisher;
};
}  // namespace wpi
