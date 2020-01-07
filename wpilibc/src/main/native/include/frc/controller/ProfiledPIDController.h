/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

#include <units/units.h>

#include "frc/controller/PIDController.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableHelper.h"
#include "frc/trajectory/TrapezoidProfile.h"

namespace frc {
namespace detail {
void ReportProfiledPIDController();
}  // namespace detail

/**
 * Implements a PID control loop whose setpoint is constrained by a trapezoid
 * profile.
 */
template <class Distance>
class ProfiledPIDController
    : public Sendable,
      public SendableHelper<ProfiledPIDController<Distance>> {
 public:
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using Acceleration_t = units::unit_t<Acceleration>;
  using State = typename TrapezoidProfile<Distance>::State;
  using Constraints = typename TrapezoidProfile<Distance>::Constraints;

  /**
   * Allocates a ProfiledPIDController with the given constants for Kp, Ki, and
   * Kd. Users should call reset() when they first start running the controller
   * to avoid unwanted behavior.
   *
   * @param Kp          The proportional coefficient.
   * @param Ki          The integral coefficient.
   * @param Kd          The derivative coefficient.
   * @param constraints Velocity and acceleration constraints for goal.
   * @param period      The period between controller updates in seconds. The
   *                    default is 20 milliseconds.
   */
  ProfiledPIDController(double Kp, double Ki, double Kd,
                        Constraints constraints, units::second_t period = 20_ms)
      : m_controller(Kp, Ki, Kd, period), m_constraints(constraints) {
    detail::ReportProfiledPIDController();
  }

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
  void SetPID(double Kp, double Ki, double Kd) {
    m_controller.SetPID(Kp, Ki, Kd);
  }

  /**
   * Sets the proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  void SetP(double Kp) { m_controller.SetP(Kp); }

  /**
   * Sets the integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  void SetI(double Ki) { m_controller.SetI(Ki); }

  /**
   * Sets the differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  void SetD(double Kd) { m_controller.SetD(Kd); }

  /**
   * Gets the proportional coefficient.
   *
   * @return proportional coefficient
   */
  double GetP() const { return m_controller.GetP(); }

  /**
   * Gets the integral coefficient.
   *
   * @return integral coefficient
   */
  double GetI() const { return m_controller.GetI(); }

  /**
   * Gets the differential coefficient.
   *
   * @return differential coefficient
   */
  double GetD() const { return m_controller.GetD(); }

  /**
   * Gets the period of this controller.
   *
   * @return The period of the controller.
   */
  units::second_t GetPeriod() const { return m_controller.GetPeriod(); }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  void SetGoal(State goal) { m_goal = goal; }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  void SetGoal(Distance_t goal) { m_goal = {goal, Velocity_t(0)}; }

  /**
   * Gets the goal for the ProfiledPIDController.
   */
  State GetGoal() const { return m_goal; }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * This will return false until at least one input value has been computed.
   */
  bool AtGoal() const { return AtSetpoint() && m_goal == m_setpoint; }

  /**
   * Set velocity and acceleration constraints for goal.
   *
   * @param constraints Velocity and acceleration constraints for goal.
   */
  void SetConstraints(Constraints constraints) { m_constraints = constraints; }

  /**
   * Returns the current setpoint of the ProfiledPIDController.
   *
   * @return The current setpoint.
   */
  State GetSetpoint() const { return m_setpoint; }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * Currently this just reports on target as the actual value passes through
   * the setpoint. Ideally it should be based on being within the tolerance for
   * some period of time.
   *
   * This will return false until at least one input value has been computed.
   */
  bool AtSetpoint() const { return m_controller.AtSetpoint(); }

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
  void EnableContinuousInput(Distance_t minimumInput, Distance_t maximumInput) {
    m_controller.EnableContinuousInput(minimumInput.template to<double>(),
                                       maximumInput.template to<double>());
  }

  /**
   * Disables continuous input.
   */
  void DisableContinuousInput() { m_controller.DisableContinuousInput(); }

  /**
   * Sets the minimum and maximum values for the integrator.
   *
   * When the cap is reached, the integrator value is added to the controller
   * output rather than the integrator value times the integral gain.
   *
   * @param minimumIntegral The minimum value of the integrator.
   * @param maximumIntegral The maximum value of the integrator.
   */
  void SetIntegratorRange(double minimumIntegral, double maximumIntegral) {
    m_controller.SetIntegratorRange(minimumIntegral, maximumIntegral);
  }

  /**
   * Sets the error which is considered tolerable for use with
   * AtSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   * @param velocityTolerance Velocity error which is tolerable.
   */
  void SetTolerance(
      Distance_t positionTolerance,
      Velocity_t velocityTolerance = std::numeric_limits<double>::infinity()) {
    m_controller.SetTolerance(positionTolerance.template to<double>(),
                              velocityTolerance.template to<double>());
  }

  /**
   * Returns the difference between the setpoint and the measurement.
   *
   * @return The error.
   */
  Distance_t GetPositionError() const {
    return Distance_t(m_controller.GetPositionError());
  }

  /**
   * Returns the change in error per second.
   */
  Velocity_t GetVelocityError() const {
    return Velocity_t(m_controller.GetVelocityError());
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   */
  double Calculate(Distance_t measurement) {
    frc::TrapezoidProfile<Distance> profile{m_constraints, m_goal, m_setpoint};
    m_setpoint = profile.Calculate(GetPeriod());
    return m_controller.Calculate(measurement.template to<double>(),
                                  m_setpoint.position.template to<double>());
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  double Calculate(Distance_t measurement, State goal) {
    SetGoal(goal);
    return Calculate(measurement);
  }
  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  double Calculate(Distance_t measurement, Distance_t goal) {
    SetGoal(goal);
    return Calculate(measurement);
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal        The new goal of the controller.
   * @param constraints Velocity and acceleration constraints for goal.
   */
  double Calculate(
      Distance_t measurement, Distance_t goal,
      typename frc::TrapezoidProfile<Distance>::Constraints constraints) {
    SetConstraints(constraints);
    return Calculate(measurement, goal);
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measurement The current measured State of the system.
   */
  void Reset(const State& measurement) {
    m_controller.Reset();
    m_setpoint = measurement;
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system.
   * @param measuredVelocity The current measured velocity of the system.
   */
  void Reset(Distance_t measuredPosition, Velocity_t measuredVelocity) {
    Reset(State{measuredPosition, measuredVelocity});
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system. The
   * velocity is assumed to be zero.
   */
  void Reset(Distance_t measuredPosition) {
    Reset(measuredPosition, Velocity_t(0));
  }

  void InitSendable(frc::SendableBuilder& builder) override {
    builder.SetSmartDashboardType("ProfiledPIDController");
    builder.AddDoubleProperty("p", [this] { return GetP(); },
                              [this](double value) { SetP(value); });
    builder.AddDoubleProperty("i", [this] { return GetI(); },
                              [this](double value) { SetI(value); });
    builder.AddDoubleProperty("d", [this] { return GetD(); },
                              [this](double value) { SetD(value); });
    builder.AddDoubleProperty(
        "goal", [this] { return GetGoal().position.template to<double>(); },
        [this](double value) { SetGoal(Distance_t{value}); });
  }

 private:
  frc2::PIDController m_controller;
  typename frc::TrapezoidProfile<Distance>::State m_goal;
  typename frc::TrapezoidProfile<Distance>::State m_setpoint;
  typename frc::TrapezoidProfile<Distance>::Constraints m_constraints;
};

}  // namespace frc
