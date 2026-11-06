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
   * Sets the gravity compensation gain for a lifting mechanism.
   *
   * These gravity-compensation settings are mutually exclusive; setting this
   * value clears the arm gravity gain.
   *
   * @param g The lifting gravity gain.
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& SetG(double g);

  /**
   * Sets the gravity compensation gain for an arm mechanism.
   *
   * These gravity-compensation settings are mutually exclusive; setting this
   * value clears the lift gravity gain.
   *
   * @param cos The arm gravity gain.
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& SetCos(double cos);

  /**
   * Sets the conversion factor that translates the selected sensor's position
   * units into absolute mechanism rotations for an arm mechanism.
   *
   * @param cosRatio The conversion factor for the arm gravity compensation
   *     ratio.
   * @return This object, for method chaining.
   */
  ExpansionHubPositionConstants& SetCosRatio(double cosRatio);

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
  wpi::nt::DoublePublisher m_gPublisher;
  wpi::nt::DoublePublisher m_cosPublisher;
  wpi::nt::DoublePublisher m_cosRatioPublisher;

  wpi::nt::BooleanPublisher m_continuousPublisher;
  wpi::nt::DoublePublisher m_continuousMinimumPublisher;
  wpi::nt::DoublePublisher m_continuousMaximumPublisher;
};
}  // namespace wpi
