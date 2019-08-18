/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/smartdashboard/SendableBuilder.h"

namespace frc {

template <typename Unit>
ProfiledPIDController<Unit>::ProfiledPIDController(
    double Kp, double Ki, double Kd,
    typename TrapezoidProfile<Unit>::Constraints constraints,
    units::second_t period)
    : m_controller(Kp, Ki, Kd, period), m_constraints(constraints) {}

template <typename Unit>
void ProfiledPIDController<Unit>::SetP(double Kp) {
  m_controller.SetP(Kp);
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetI(double Ki) {
  m_controller.SetI(Ki);
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetD(double Kd) {
  m_controller.SetD(Kd);
}

template <typename Unit>
double ProfiledPIDController<Unit>::GetP() const {
  return m_controller.GetP();
}

template <typename Unit>
double ProfiledPIDController<Unit>::GetI() const {
  return m_controller.GetI();
}

template <typename Unit>
double ProfiledPIDController<Unit>::GetD() const {
  return m_controller.GetD();
}

template <typename Unit>
units::second_t ProfiledPIDController<Unit>::GetPeriod() const {
  return m_controller.GetPeriod();
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetGoal(Unit goal) {
  m_goal = {goal, UnitPerSec{0.0}};
}

template <typename Unit>
Unit ProfiledPIDController<Unit>::GetGoal() const {
  return m_goal.position;
}

template <typename Unit>
bool ProfiledPIDController<Unit>::AtGoal() const {
  return AtSetpoint() && m_goal == m_setpoint;
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetConstraints(
    typename TrapezoidProfile<Unit>::Constraints constraints) {
  m_constraints = constraints;
}

template <typename Unit>
Unit ProfiledPIDController<Unit>::GetSetpoint() const {
  return m_controller.GetSetpoint();
}

template <typename Unit>
bool ProfiledPIDController<Unit>::AtSetpoint() const {
  return m_controller.AtSetpoint();
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetInputRange(Unit minimumInput,
                                                Unit maximumInput) {
  m_controller.SetInputRange(minimumInput, maximumInput);
}

template <typename Unit>
void ProfiledPIDController<Unit>::EnableContinuousInput(Unit minimumInput,
                                                        Unit maximumInput) {
  m_controller.EnableContinuousInput(minimumInput, maximumInput);
}

template <typename Unit>
void ProfiledPIDController<Unit>::DisableContinuousInput() {
  m_controller.DisableContinuousInput();
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetOutputRange(double minimumOutput,
                                                 double maximumOutput) {
  m_controller.SetOutputRange(minimumOutput, maximumOutput);
}

template <typename Unit>
void ProfiledPIDController<Unit>::SetAbsoluteTolerance(
    Unit positionTolerance, UnitPerSec velocityTolerance) {
  m_controller.SetAbsoluteTolerance(positionTolerance, velocityTolerance);
}

template <typename Unit>
Unit ProfiledPIDController<Unit>::GetPositionError() const {
  return m_controller.GetPositionError();
}

template <typename Unit>
typename ProfiledPIDController<Unit>::UnitPerSec
ProfiledPIDController<Unit>::GetVelocityError() const {
  return m_controller.GetVelocityError();
}

template <typename Unit>
double ProfiledPIDController<Unit>::Calculate(Unit measurement) {
  TrapezoidProfile<Unit> profile{m_constraints, m_goal, m_setpoint};
  m_setpoint = profile.Calculate(GetPeriod());
  return m_controller.Calculate(measurement, m_setpoint.position);
}

template <typename Unit>
double ProfiledPIDController<Unit>::Calculate(Unit measurement, Unit goal) {
  SetGoal(goal);
  return Calculate(measurement);
}

template <typename Unit>
double ProfiledPIDController<Unit>::Calculate(
    Unit measurement, Unit goal,
    typename TrapezoidProfile<Unit>::Constraints constraints) {
  SetConstraints(constraints);
  return Calculate(measurement, goal);
}

template <typename Unit>
void ProfiledPIDController<Unit>::Reset() {
  m_controller.Reset();
}

template <typename Unit>
void ProfiledPIDController<Unit>::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("ProfiledPIDController");
  builder.AddDoubleProperty("p", [this] { return GetP(); },
                            [this](double value) { SetP(value); });
  builder.AddDoubleProperty("i", [this] { return GetI(); },
                            [this](double value) { SetI(value); });
  builder.AddDoubleProperty("d", [this] { return GetD(); },
                            [this](double value) { SetD(value); });
  builder.AddDoubleProperty("goal",
                            [this] { return GetGoal().template to<double>(); },
                            [this](double value) { SetGoal(Unit{value}); });
}

}  // namespace frc
