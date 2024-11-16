// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

#include <wpi/SymbolExports.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/MathUtil.h"
#include "frc/controller/PIDController.h"
#include "frc/trajectory/TrapezoidProfile.h"
#include "units/time.h"

namespace frc {
namespace detail {
WPILIB_DLLEXPORT
int IncrementAndGetProfiledPIDControllerInstances();
}  // namespace detail

/**
 * Implements a PID control loop whose setpoint is constrained by a trapezoid
 * profile.
 */
template <class Distance>
class ProfiledPIDController
    : public wpi::Sendable,
      public wpi::SendableHelper<ProfiledPIDController<Distance>> {
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
   * @param Kp          The proportional coefficient. Must be >= 0.
   * @param Ki          The integral coefficient. Must be >= 0.
   * @param Kd          The derivative coefficient. Must be >= 0.
   * @param constraints Velocity and acceleration constraints for goal.
   * @param period      The period between controller updates in seconds. The
   *                    default is 20 milliseconds. Must be positive.
   */
  constexpr ProfiledPIDController(double Kp, double Ki, double Kd,
                                  Constraints constraints,
                                  units::second_t period = 20_ms)
      : m_controller{Kp, Ki, Kd, period},
        m_constraints{constraints},
        m_profile{m_constraints} {
    if (!std::is_constant_evaluated()) {
      int instances = detail::IncrementAndGetProfiledPIDControllerInstances();
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kController_ProfiledPIDController, instances);
      wpi::SendableRegistry::Add(this, "ProfiledPIDController", instances);
    }
  }

  constexpr ~ProfiledPIDController() override = default;

  constexpr ProfiledPIDController(const ProfiledPIDController&) = default;
  constexpr ProfiledPIDController& operator=(const ProfiledPIDController&) =
      default;
  constexpr ProfiledPIDController(ProfiledPIDController&&) = default;
  constexpr ProfiledPIDController& operator=(ProfiledPIDController&&) = default;

  /**
   * Sets the PID Controller gain parameters.
   *
   * Sets the proportional, integral, and differential coefficients.
   *
   * @param Kp The proportional coefficient. Must be >= 0.
   * @param Ki The integral coefficient. Must be >= 0.
   * @param Kd The differential coefficient. Must be >= 0.
   */
  constexpr void SetPID(double Kp, double Ki, double Kd) {
    m_controller.SetPID(Kp, Ki, Kd);
  }

  /**
   * Sets the proportional coefficient of the PID controller gain.
   *
   * @param Kp The proportional coefficient. Must be >= 0.
   */
  constexpr void SetP(double Kp) { m_controller.SetP(Kp); }

  /**
   * Sets the integral coefficient of the PID controller gain.
   *
   * @param Ki The integral coefficient. Must be >= 0.
   */
  constexpr void SetI(double Ki) { m_controller.SetI(Ki); }

  /**
   * Sets the differential coefficient of the PID controller gain.
   *
   * @param Kd The differential coefficient. Must be >= 0.
   */
  constexpr void SetD(double Kd) { m_controller.SetD(Kd); }

  /**
   * Sets the IZone range. When the absolute value of the position error is
   * greater than IZone, the total accumulated error will reset to zero,
   * disabling integral gain until the absolute value of the position error is
   * less than IZone. This is used to prevent integral windup. Must be
   * non-negative. Passing a value of zero will effectively disable integral
   * gain. Passing a value of infinity disables IZone functionality.
   *
   * @param iZone Maximum magnitude of error to allow integral control. Must be
   *   >= 0.
   */
  constexpr void SetIZone(double iZone) { m_controller.SetIZone(iZone); }

  /**
   * Gets the proportional coefficient.
   *
   * @return proportional coefficient
   */
  constexpr double GetP() const { return m_controller.GetP(); }

  /**
   * Gets the integral coefficient.
   *
   * @return integral coefficient
   */
  constexpr double GetI() const { return m_controller.GetI(); }

  /**
   * Gets the differential coefficient.
   *
   * @return differential coefficient
   */
  constexpr double GetD() const { return m_controller.GetD(); }

  /**
   * Get the IZone range.
   *
   * @return Maximum magnitude of error to allow integral control.
   */
  constexpr double GetIZone() const { return m_controller.GetIZone(); }

  /**
   * Gets the period of this controller.
   *
   * @return The period of the controller.
   */
  constexpr units::second_t GetPeriod() const {
    return m_controller.GetPeriod();
  }

  /**
   * Gets the position tolerance of this controller.
   *
   * @return The position tolerance of the controller.
   */
  constexpr double GetPositionTolerance() const {
    return m_controller.GetErrorTolerance();
  }

  /**
   * Gets the velocity tolerance of this controller.
   *
   * @return The velocity tolerance of the controller.
   */
  constexpr double GetVelocityTolerance() const {
    return m_controller.GetErrorDerivativeTolerance();
  }

  /**
   * Gets the accumulated error used in the integral calculation of this
   * controller.
   *
   * @return The accumulated error of this controller.
   */
  constexpr double GetAccumulatedError() const {
    return m_controller.GetAccumulatedError();
  }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  constexpr void SetGoal(State goal) { m_goal = goal; }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired unprofiled setpoint.
   */
  constexpr void SetGoal(Distance_t goal) { m_goal = {goal, Velocity_t{0}}; }

  /**
   * Gets the goal for the ProfiledPIDController.
   */
  constexpr State GetGoal() const { return m_goal; }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * This will return false until at least one input value has been computed.
   */
  constexpr bool AtGoal() const { return AtSetpoint() && m_goal == m_setpoint; }

  /**
   * Set velocity and acceleration constraints for goal.
   *
   * @param constraints Velocity and acceleration constraints for goal.
   */
  constexpr void SetConstraints(Constraints constraints) {
    m_constraints = constraints;
    m_profile = TrapezoidProfile<Distance>{m_constraints};
  }

  /**
   * Get the velocity and acceleration constraints for this controller.
   * @return Velocity and acceleration constraints.
   */
  constexpr Constraints GetConstraints() { return m_constraints; }

  /**
   * Returns the current setpoint of the ProfiledPIDController.
   *
   * @return The current setpoint.
   */
  constexpr State GetSetpoint() const { return m_setpoint; }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * Currently this just reports on target as the actual value passes through
   * the setpoint. Ideally it should be based on being within the tolerance for
   * some period of time.
   *
   * This will return false until at least one input value has been computed.
   */
  constexpr bool AtSetpoint() const { return m_controller.AtSetpoint(); }

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
  constexpr void EnableContinuousInput(Distance_t minimumInput,
                                       Distance_t maximumInput) {
    m_controller.EnableContinuousInput(minimumInput.value(),
                                       maximumInput.value());
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
  }

  /**
   * Disables continuous input.
   */
  constexpr void DisableContinuousInput() {
    m_controller.DisableContinuousInput();
  }

  /**
   * Sets the minimum and maximum contributions of the integral term.
   *
   * The internal integrator is clamped so that the integral term's contribution
   * to the output stays between minimumIntegral and maximumIntegral. This
   * prevents integral windup.
   *
   * @param minimumIntegral The minimum contribution of the integral term.
   * @param maximumIntegral The maximum contribution of the integral term.
   */
  constexpr void SetIntegratorRange(double minimumIntegral,
                                    double maximumIntegral) {
    m_controller.SetIntegratorRange(minimumIntegral, maximumIntegral);
  }

  /**
   * Sets the error which is considered tolerable for use with
   * AtSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   * @param velocityTolerance Velocity error which is tolerable.
   */
  constexpr void SetTolerance(Distance_t positionTolerance,
                              Velocity_t velocityTolerance = Velocity_t{
                                  std::numeric_limits<double>::infinity()}) {
    m_controller.SetTolerance(positionTolerance.value(),
                              velocityTolerance.value());
  }

  /**
   * Returns the difference between the setpoint and the measurement.
   *
   * @return The error.
   */
  constexpr Distance_t GetPositionError() const {
    return Distance_t{m_controller.GetError()};
  }

  /**
   * Returns the change in error per second.
   */
  constexpr Velocity_t GetVelocityError() const {
    return Velocity_t{m_controller.GetErrorDerivative()};
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   */
  constexpr double Calculate(Distance_t measurement) {
    if (m_controller.IsContinuousInputEnabled()) {
      // Get error which is smallest distance between goal and measurement
      auto errorBound = (m_maximumInput - m_minimumInput) / 2.0;
      auto goalMinDistance = frc::InputModulus<Distance_t>(
          m_goal.position - measurement, -errorBound, errorBound);
      auto setpointMinDistance = frc::InputModulus<Distance_t>(
          m_setpoint.position - measurement, -errorBound, errorBound);

      // Recompute the profile goal with the smallest error, thus giving the
      // shortest path. The goal may be outside the input range after this
      // operation, but that's OK because the controller will still go there and
      // report an error of zero. In other words, the setpoint only needs to be
      // offset from the measurement by the input range modulus; they don't need
      // to be equal.
      m_goal.position = goalMinDistance + measurement;
      m_setpoint.position = setpointMinDistance + measurement;
    }

    m_setpoint = m_profile.Calculate(GetPeriod(), m_setpoint, m_goal);
    return m_controller.Calculate(measurement.value(),
                                  m_setpoint.position.value());
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  constexpr double Calculate(Distance_t measurement, State goal) {
    SetGoal(goal);
    return Calculate(measurement);
  }
  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   */
  constexpr double Calculate(Distance_t measurement, Distance_t goal) {
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
  constexpr double Calculate(
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
  constexpr void Reset(const State& measurement) {
    m_controller.Reset();
    m_setpoint = measurement;
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system.
   * @param measuredVelocity The current measured velocity of the system.
   */
  constexpr void Reset(Distance_t measuredPosition,
                       Velocity_t measuredVelocity) {
    Reset(State{measuredPosition, measuredVelocity});
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system. The
   * velocity is assumed to be zero.
   */
  constexpr void Reset(Distance_t measuredPosition) {
    Reset(measuredPosition, Velocity_t{0});
  }

  void InitSendable(wpi::SendableBuilder& builder) override {
    builder.SetSmartDashboardType("ProfiledPIDController");
    builder.AddDoubleProperty(
        "p", [this] { return GetP(); }, [this](double value) { SetP(value); });
    builder.AddDoubleProperty(
        "i", [this] { return GetI(); }, [this](double value) { SetI(value); });
    builder.AddDoubleProperty(
        "d", [this] { return GetD(); }, [this](double value) { SetD(value); });
    builder.AddDoubleProperty(
        "izone", [this] { return GetIZone(); },
        [this](double value) { SetIZone(value); });
    builder.AddDoubleProperty(
        "maxVelocity", [this] { return GetConstraints().maxVelocity.value(); },
        [this](double value) {
          SetConstraints(
              Constraints{Velocity_t{value}, GetConstraints().maxAcceleration});
        });
    builder.AddDoubleProperty(
        "maxAcceleration",
        [this] { return GetConstraints().maxAcceleration.value(); },
        [this](double value) {
          SetConstraints(
              Constraints{GetConstraints().maxVelocity, Acceleration_t{value}});
        });
    builder.AddDoubleProperty(
        "goal", [this] { return GetGoal().position.value(); },
        [this](double value) { SetGoal(Distance_t{value}); });
  }

 private:
  PIDController m_controller;
  Distance_t m_minimumInput{0};
  Distance_t m_maximumInput{0};

  typename frc::TrapezoidProfile<Distance>::Constraints m_constraints;
  TrapezoidProfile<Distance> m_profile;
  typename frc::TrapezoidProfile<Distance>::State m_goal;
  typename frc::TrapezoidProfile<Distance>::State m_setpoint;
};

}  // namespace frc
