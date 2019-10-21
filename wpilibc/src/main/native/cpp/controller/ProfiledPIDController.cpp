/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/ProfiledPIDController.h"

#include <algorithm>
#include <cmath>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

ProfiledPIDController::ProfiledPIDController(
    double Kp, double Ki, double Kd,
    frc::TrapezoidProfile::Constraints constraints, units::second_t period)
    : m_controller(Kp, Ki, Kd, period), m_constraints(constraints) {}

void ProfiledPIDController::SetP(double Kp) { m_controller.SetP(Kp); }

void ProfiledPIDController::SetI(double Ki) { m_controller.SetI(Ki); }

void ProfiledPIDController::SetD(double Kd) { m_controller.SetD(Kd); }

double ProfiledPIDController::GetP() const { return m_controller.GetP(); }

double ProfiledPIDController::GetI() const { return m_controller.GetI(); }

double ProfiledPIDController::GetD() const { return m_controller.GetD(); }

units::second_t ProfiledPIDController::GetPeriod() const {
  return m_controller.GetPeriod();
}

void ProfiledPIDController::SetGoal(TrapezoidProfile::State goal) {
  m_goal = goal;
}

void ProfiledPIDController::SetGoal(units::meter_t goal) {
  m_goal = {goal, 0_mps};
}

TrapezoidProfile::State ProfiledPIDController::GetGoal() const {
  return m_goal;
}

bool ProfiledPIDController::AtGoal() const {
  return AtSetpoint() && m_goal == m_setpoint;
}

void ProfiledPIDController::SetConstraints(
    frc::TrapezoidProfile::Constraints constraints) {
  m_constraints = constraints;
}

TrapezoidProfile::State ProfiledPIDController::GetSetpoint() const {
  return m_setpoint;
}

bool ProfiledPIDController::AtSetpoint() const {
  return m_controller.AtSetpoint();
}

void ProfiledPIDController::EnableContinuousInput(double minimumInput,
                                                  double maximumInput) {
  m_controller.EnableContinuousInput(minimumInput, maximumInput);
}

void ProfiledPIDController::DisableContinuousInput() {
  m_controller.DisableContinuousInput();
}

void ProfiledPIDController::SetIntegratorRange(double minimumIntegral,
                                               double maximumIntegral) {
  m_controller.SetIntegratorRange(minimumIntegral, maximumIntegral);
}

void ProfiledPIDController::SetTolerance(double positionTolerance,
                                         double velocityTolerance) {
  m_controller.SetTolerance(positionTolerance, velocityTolerance);
}

double ProfiledPIDController::GetPositionError() const {
  return m_controller.GetPositionError();
}

double ProfiledPIDController::GetVelocityError() const {
  return m_controller.GetVelocityError();
}

double ProfiledPIDController::Calculate(units::meter_t measurement) {
  frc::TrapezoidProfile profile{m_constraints, m_goal, m_setpoint};
  m_setpoint = profile.Calculate(GetPeriod());
  return m_controller.Calculate(measurement.to<double>(),
                                m_setpoint.position.to<double>());
}

double ProfiledPIDController::Calculate(units::meter_t measurement,
                                        TrapezoidProfile::State goal) {
  SetGoal(goal);
  return Calculate(measurement);
}

double ProfiledPIDController::Calculate(units::meter_t measurement,
                                        units::meter_t goal) {
  SetGoal(goal);
  return Calculate(measurement);
}

double ProfiledPIDController::Calculate(
    units::meter_t measurement, units::meter_t goal,
    frc::TrapezoidProfile::Constraints constraints) {
  SetConstraints(constraints);
  return Calculate(measurement, goal);
}

void ProfiledPIDController::Reset() { m_controller.Reset(); }

void ProfiledPIDController::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("ProfiledPIDController");
  builder.AddDoubleProperty("p", [this] { return GetP(); },
                            [this](double value) { SetP(value); });
  builder.AddDoubleProperty("i", [this] { return GetI(); },
                            [this](double value) { SetI(value); });
  builder.AddDoubleProperty("d", [this] { return GetD(); },
                            [this](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "goal", [this] { return GetGoal().position.to<double>(); },
      [this](double value) { SetGoal(units::meter_t{value}); });
}
