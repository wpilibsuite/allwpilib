/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <limits>

#include <units/units.h>

#include "frc/controller/PIDController.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/trajectory/TrapezoidProfile.h"

namespace frc {

/**
 * Implements a PID control loop whose setpoint is constrained by a trapezoid
 * profile.
 */
class ProfiledPIDController : public Sendable,
                              public SendableHelper<ProfiledPIDController> {
 public:
  /**
   * Allocates a ProfiledPIDController with the given constants for Kp, Ki, and
   * Kd.
   *
   * @param Kp          The proportional coefficient.
   * @param Ki          The integral coefficient.
   * @param Kd          The derivative coefficient.
   * @param constraints Velocity and acceleration constraints for goal.
   * @param period      The period between controller updates in seconds. The
   *                    default is 20 milliseconds.
   */
  ProfiledPIDController(double Kp, double Ki, double Kd,
                        frc::TrapezoidProfile::Constraints constraints,
                        units::second_t period = 20_ms);

  ~ProfiledPIDController() override = default;

  ProfiledPIDController(const ProfiledPIDController&) = default;
  ProfiledPIDController& operator=(const ProfiledPIDController&) = default;
  ProfiledPIDController(ProfiledPIDController&&) = default;
  ProfiledPIDController& operator=(ProfiledPIDController&&) = default;

  /**
   * Sets the PID Controller gain parameters.
   *
   * Sets the proportional, integral, and differential coefficients.
   *
   * @param Kp Proportional coefficient
   * @param Ki Integral coefficient
   * @param Kd Differential coefficient
   */
  void SetPID(double Kp, double Ki, double Kd);

  /**
   * Sets the proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  void SetP(double Kp);

  /**
   * Sets the integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  void SetI(double Ki);

  /**
   * Sets the differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  void SetD(double Kd);

  /**
   * Gets the proportional coefficient.
   *
   * @return proportional coefficient
   */
  double GetP() const;

  /**
   * Gets the integral coefficient.
   *
   * @return integral coefficient
   */
  double GetI() const;

  /**
   * Gets the differential coefficient.
   *
   * @return differential coefficient
   */
  double GetD() const;

  /**
   * Gets the period of this controller.
   *
   * @return The period of the controller.
   */
  units::second_t GetPeriod() const;

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  void SetGoal(TrapezoidProfile::State goal);

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  void SetGoal(units::meter_t goal);

  /**
   * Gets the goal for the ProfiledPIDController.
   */
  TrapezoidProfile::State GetGoal() const;

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * This will return false until at least one input value has been computed.
   */
  bool AtGoal() const;

  /**
   * Set velocity and acceleration constraints for goal.
   *
   * @param constraints Velocity and acceleration constraints for goal.
   */
  void SetConstraints(frc::TrapezoidProfile::Constraints constraints);

  /**
   * Returns the current setpoint of the ProfiledPIDController.
   *
   * @return The current setpoint.
   */
  TrapezoidProfile::State GetSetpoint() const;

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * Currently this just reports on target as the actual value passes through
   * the setpoint. Ideally it should be based on being within the tolerance for
   * some period of time.
   *
   * This will return false until at least one input value has been computed.
   */
  bool AtSetpoint() const;

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
  void EnableContinuousInput(double minimumInput, double maximumInput);

  /**
   * Disables continuous input.
   */
  void DisableContinuousInput();

  /**
   * Sets the minimum and maximum values for the integrator.
   *
   * When the cap is reached, the integrator value is added to the controller
   * output rather than the integrator value times the integral gain.
   *
   * @param minimumIntegral The minimum value of the integrator.
   * @param maximumIntegral The maximum value of the integrator.
   */
  void SetIntegratorRange(double minimumIntegral, double maximumIntegral);

  /**
   * Sets the error which is considered tolerable for use with
   * AtSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   * @param velocityTolerance Velocity error which is tolerable.
   */
  void SetTolerance(
      double positionTolerance,
      double velocityTolerance = std::numeric_limits<double>::infinity());

  /**
   * Returns the difference between the setpoint and the measurement.
   *
   * @return The error.
   */
  double GetPositionError() const;

  /**
   * Returns the change in error per second.
   */
  double GetVelocityError() const;

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   */
  double Calculate(units::meter_t measurement);

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  double Calculate(units::meter_t measurement, TrapezoidProfile::State goal);

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  double Calculate(units::meter_t measurement, units::meter_t goal);

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal        The new goal of the controller.
   * @param constraints Velocity and acceleration constraints for goal.
   */
  double Calculate(units::meter_t measurement, units::meter_t goal,
                   frc::TrapezoidProfile::Constraints constraints);

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  void Reset();

  void InitSendable(frc::SendableBuilder& builder) override;

 private:
  frc2::PIDController m_controller;
  frc::TrapezoidProfile::State m_goal;
  frc::TrapezoidProfile::State m_setpoint;
  frc::TrapezoidProfile::Constraints m_constraints;
};

}  // namespace frc
