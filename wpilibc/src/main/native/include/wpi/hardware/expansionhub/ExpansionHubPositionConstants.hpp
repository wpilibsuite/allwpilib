// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"

namespace wpi {
class ExpansionHubMotor;

/** This class contains feedback and feedforward constants for an ExpansionHub
 * motor. */
class ExpansionHubPositionConstants {
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
  ExpansionHubPositionConstants& SetPID(double p, double i, double d);

  /**
   * Sets the feed forward gains to the specified values.
   *
   * The units should be radians for angular systems and meters for linear
   * systems.
   *
   * The motor control period is 10ms
   *
   * @param s The static gain in volts.
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& SetS(double s);

  /**
   * Enables continuous input.
   *
   * Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& EnableContinuousInput(double minimumInput,
                                                       double maximumInput);

  /**
   * Disable continuous input mode.
   *
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& DisableContinuousInput();

  ExpansionHubPositionConstants(ExpansionHubPositionConstants&) = delete;
  ExpansionHubPositionConstants& operator=(ExpansionHubPositionConstants&) =
      delete;

  ExpansionHubPositionConstants(ExpansionHubPositionConstants&&) = default;
  ExpansionHubPositionConstants& operator=(ExpansionHubPositionConstants&&) =
      default;

  friend class ExpansionHubMotor;

 private:
  ExpansionHubPositionConstants(int hubNumber, int motorNumber);

  wpi::nt::DoublePublisher m_pPublisher;
  wpi::nt::DoublePublisher m_iPublisher;
  wpi::nt::DoublePublisher m_dPublisher;

  wpi::nt::DoublePublisher m_sPublisher;

  wpi::nt::BooleanPublisher m_continuousPublisher;
  wpi::nt::DoublePublisher m_continuousMinimumPublisher;
  wpi::nt::DoublePublisher m_continuousMaximumPublisher;
};
}  // namespace wpi
