/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/Twine.h>

#include "frc/PIDController.h"
#include "frc/PIDOutput.h"
#include "frc/PIDSource.h"
#include "frc/commands/Subsystem.h"

namespace frc {

/**
 * This class is designed to handle the case where there is a Subsystem which
 * uses a single PIDController almost constantly (for instance, an elevator
 * which attempts to stay at a constant height).
 *
 * It provides some convenience methods to run an internal PIDController. It
 * also allows access to the internal PIDController in order to give total
 * control to the programmer.
 */
class PIDSubsystem : public Subsystem, public PIDOutput, public PIDSource {
 public:
  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * @param name the name
   * @param p    the proportional value
   * @param i    the integral value
   * @param d    the derivative value
   */
  PIDSubsystem(const wpi::Twine& name, double p, double i, double d);

  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * @param name the name
   * @param p    the proportional value
   * @param i    the integral value
   * @param d    the derivative value
   * @param f    the feedforward value
   */
  PIDSubsystem(const wpi::Twine& name, double p, double i, double d, double f);

  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * It will also space the time between PID loop calculations to be equal to
   * the given period.
   *
   * @param name   the name
   * @param p      the proportional value
   * @param i      the integral value
   * @param d      the derivative value
   * @param f      the feedfoward value
   * @param period the time (in seconds) between calculations
   */
  PIDSubsystem(const wpi::Twine& name, double p, double i, double d, double f,
               double period);

  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * It will use the class name as its name.
   *
   * @param p the proportional value
   * @param i the integral value
   * @param d the derivative value
   */
  PIDSubsystem(double p, double i, double d);

  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * It will use the class name as its name.
   *
   * @param p the proportional value
   * @param i the integral value
   * @param d the derivative value
   * @param f the feedforward value
   */
  PIDSubsystem(double p, double i, double d, double f);

  /**
   * Instantiates a PIDSubsystem that will use the given P, I, and D values.
   *
   * It will use the class name as its name. It will also space the time
   * between PID loop calculations to be equal to the given period.
   *
   * @param p      the proportional value
   * @param i      the integral value
   * @param d      the derivative value
   * @param f      the feedforward value
   * @param period the time (in seconds) between calculations
   */
  PIDSubsystem(double p, double i, double d, double f, double period);

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

  // PIDOutput interface
  void PIDWrite(double output) override;

  // PIDSource interface

  double PIDGet() override;

  /**
   * Sets the setpoint to the given value.
   *
   * If SetRange() was called, then the given setpoint will be trimmed to fit
   * within the range.
   *
   * @param setpoint the new setpoint
   */
  void SetSetpoint(double setpoint);

  /**
   * Adds the given value to the setpoint.
   *
   * If SetRange() was used, then the bounds will still be honored by this
   * method.
   *
   * @param deltaSetpoint the change in the setpoint
   */
  void SetSetpointRelative(double deltaSetpoint);

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the output
   */
  void SetInputRange(double minimumInput, double maximumInput);

  /**
   * Sets the maximum and minimum values to write.
   *
   * @param minimumOutput the minimum value to write to the output
   * @param maximumOutput the maximum value to write to the output
   */
  void SetOutputRange(double minimumOutput, double maximumOutput);

  /**
   * Return the current setpoint.
   *
   * @return The current setpoint
   */
  double GetSetpoint();

  /**
   * Returns the current position.
   *
   * @return the current position
   */
  double GetPosition();

  /**
   * Returns the current rate.
   *
   * @return the current rate
   */
  double GetRate();

  /**
   * Set the absolute error which is considered tolerable for use with
   * OnTarget.
   *
   * @param absValue absolute error which is tolerable
   */
  virtual void SetAbsoluteTolerance(double absValue);

  /**
   * Set the percentage error which is considered tolerable for use with
   * OnTarget().
   *
   * @param percent percentage error which is tolerable
   */
  virtual void SetPercentTolerance(double percent);

  /**
   * Return true if the error is within the percentage of the total input range,
   * determined by SetTolerance().
   *
   * This asssumes that the maximum and minimum input were set using SetInput().
   * Use OnTarget() in the IsFinished() method of commands that use this
   * subsystem.
   *
   * Currently this just reports on target as the actual value passes through
   * the setpoint. Ideally it should be based on being within the tolerance for
   * some period of time.
   *
   * @return True if the error is within the percentage tolerance of the input
   *         range
   */
  virtual bool OnTarget() const;

 protected:
  /**
   * Returns the PIDController used by this PIDSubsystem.
   *
   * Use this if you would like to fine tune the PID loop.
   *
   * @return The PIDController used by this PIDSubsystem
   */
  std::shared_ptr<PIDController> GetPIDController();

  virtual double ReturnPIDInput() = 0;
  virtual void UsePIDOutput(double output) = 0;

 private:
  // The internal PIDController
  std::shared_ptr<PIDController> m_controller;
};

}  // namespace frc
