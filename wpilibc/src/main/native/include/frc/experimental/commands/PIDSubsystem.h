/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <limits>

#include <wpi/Twine.h>

#include "frc/commands/Subsystem.h"
#include "frc/experimental/controller/ControllerOutput.h"
#include "frc/experimental/controller/ControllerRunner.h"
#include "frc/experimental/controller/MeasurementSource.h"
#include "frc/experimental/controller/PIDController.h"

namespace frc {
namespace experimental {

// TODO: remove when this class is moved into frc namespace
using frc::Subsystem;

/**
 * This class is designed to handle the case where there is a Subsystem which
 * uses a single PIDController almost constantly (for instance, an elevator
 * which attempts to stay at a constant height).
 *
 * It provides some convenience methods to run an internal PIDController. It
 * also allows access to the internal PIDController in order to give total
 * control to the programmer.
 */
class PIDSubsystem : public Subsystem,
                     public ControllerOutput,
                     public MeasurementSource {
 public:
  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * @param name The name.
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  PIDSubsystem(const wpi::Twine& name, double Kp, double Ki, double Kd);

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   */
  PIDSubsystem(const wpi::Twine& name, double Kp, double Ki, double Kd,
               std::function<double()> feedforward);

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * It will also space the time between PID loop calculations to be equal to
   * the given period.
   *
   * @param name        The name.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedfoward function.
   * @param period      The time (in seconds) between calculations.
   */
  PIDSubsystem(const wpi::Twine& name, double Kp, double Ki, double Kd,
               std::function<double()> feedforward, double period);

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * It will use the class name as its name.
   *
   * @param Kp The proportional value.
   * @param Ki The integral value.
   * @param Kd The derivative value.
   */
  PIDSubsystem(double Kp, double Ki, double Kd);

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * It will use the class name as its name.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   */
  PIDSubsystem(double Kp, double Ki, double Kd,
               std::function<double()> feedforward);

  /**
   * Instantiates a PIDSubsystem that will use the given Kp, Ki, and Kd values.
   *
   * It will use the class name as its name. It will also space the time
   * between PID loop calculations to be equal to the given period.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The feedforward function.
   * @param period      The time (in seconds) between calculations.
   */
  PIDSubsystem(double Kp, double Ki, double Kd,
               std::function<double()> feedforward, double period);

  ~PIDSubsystem() override = default;

  PIDSubsystem(PIDSubsystem&&) = default;
  PIDSubsystem& operator=(PIDSubsystem&&) = default;

  /**
   * Enables the internal PIDController.
   */
  void Enable();

  /**
   * Disables the internal PIDController.
   */
  void Disable();

  // ControllerOutput interface
  void SetOutput(double output) override = 0;

  // MeasurementSource interface
  double GetMeasurement() const override = 0;

  /**
   * Sets the reference to the given value.
   *
   * If SetRange() was called, then the given setpoint will be trimmed to fit
   * within the range.
   *
   * @param reference The new reference.
   */
  void SetReference(double reference);

  /**
   * Adds the given value to the reference.
   *
   * If SetRange() was used, then the bounds will still be honored by this
   * method.
   *
   * @param deltaReference The change in the reference.
   */
  void SetReferenceRelative(double deltaReference);

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the output.
   */
  void SetInputRange(double minimumInput, double maximumInput);

  /**
   * Sets the maximum and minimum values to write.
   *
   * @param minimumOutput The minimum value to write to the output.
   * @param maximumOutput The maximum value to write to the output.
   */
  void SetOutputRange(double minimumOutput, double maximumOutput);

  /**
   * Return the current reference.
   *
   * @return The current reference.
   */
  double GetReference() const;

  /**
   * Set the absolute error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance      Absolute error which is tolerable.
   * @param deltaTolerance Change in absolute error per second which is
   *                       tolerable.
   */
  void SetAbsoluteTolerance(
      double tolerance,
      double deltaTolerance = std::numeric_limits<double>::infinity());

  /**
   * Set the percent error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance      Percent error which is tolerable.
   * @param deltaTolerance Change in percent error per second which is
   *                       tolerable.
   */
  void SetPercentTolerance(
      double tolerance,
      double deltaTolerance = std::numeric_limits<double>::infinity());

  /**
   * Return true if the error is within the tolerance specified by
   * SetTolerance().
   *
   * This asssumes that the maximum and minimum input were set using
   * SetInputRange(). Use AtReference() in the IsFinished() method of commands
   * that use this subsystem.
   */
  bool AtReference() const;

 protected:
  /**
   * Returns the PIDController used by this PIDSubsystem.
   *
   * Use this if you would like to fine tune the PID loop.
   *
   * @return The PIDController used by this PIDSubsystem
   */
  PIDController& GetPIDController();

 private:
  // The internal PIDController
  PIDController m_controller;
  ControllerRunner m_runner;
};

}  // namespace experimental
}  // namespace frc
