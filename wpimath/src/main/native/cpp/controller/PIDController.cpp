// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/PIDController.h"

#include <algorithm>
#include <cmath>

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "wpimath/MathShared.h"

using namespace frc2;

PIDController::PIDController(double Kp, double Ki, double Kd,
                             units::second_t period)
    : m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  if (period <= 0_s) {
    wpi::math::MathSharedStore::ReportError(
        "Controller period must be a non-zero positive number, got {}!",
        period.value());
    m_period = 20_ms;
    wpi::math::MathSharedStore::ReportWarning(
        "Controller period defaulted to 20ms.");
  }
  static int instances = 0;
  instances++;

  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kController_PIDController2, instances);
  wpi::SendableRegistry::Add(this, "PIDController", instances);
}

void PIDController::SetPID(double Kp, double Ki, double Kd) {
  m_Kp = Kp;
  m_Ki = Ki;
  m_Kd = Kd;
}

void PIDController::SetP(double Kp) {
  m_Kp = Kp;
}

void PIDController::SetI(double Ki) {
  m_Ki = Ki;
}

void PIDController::SetD(double Kd) {
  m_Kd = Kd;
}

double PIDController::GetP() const {
  return m_Kp;
}

double PIDController::GetI() const {
  return m_Ki;
}

double PIDController::GetD() const {
  return m_Kd;
}

units::second_t PIDController::GetPeriod() const {
  return m_period;
}

double PIDController::GetPositionTolerance() const {
  return m_positionTolerance;
}

double PIDController::GetVelocityTolerance() const {
  return m_velocityTolerance;
}

void PIDController::SetSetpoint(double setpoint) {
  m_setpoint = setpoint;

  if (m_continuous) {
    double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
    m_positionError =
        frc::InputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
  } else {
    m_positionError = m_setpoint - m_measurement;
  }

  m_velocityError = (m_positionError - m_prevError) / m_period.value();
}

double PIDController::GetSetpoint() const {
  return m_setpoint;
}

bool PIDController::AtSetpoint() const {
  return std::abs(m_positionError) < m_positionTolerance &&
         std::abs(m_velocityError) < m_velocityTolerance;
}

void PIDController::EnableContinuousInput(double minimumInput,
                                          double maximumInput) {
  m_continuous = true;
  m_minimumInput = minimumInput;
  m_maximumInput = maximumInput;
}

void PIDController::DisableContinuousInput() {
  m_continuous = false;
}

bool PIDController::IsContinuousInputEnabled() const {
  return m_continuous;
}

void PIDController::SetIntegratorRange(double minimumIntegral,
                                       double maximumIntegral) {
  m_minimumIntegral = minimumIntegral;
  m_maximumIntegral = maximumIntegral;
}

void PIDController::SetTolerance(double positionTolerance,
                                 double velocityTolerance) {
  m_positionTolerance = positionTolerance;
  m_velocityTolerance = velocityTolerance;
}

double PIDController::GetPositionError() const {
  return m_positionError;
}

double PIDController::GetVelocityError() const {
  return m_velocityError;
}

double PIDController::Calculate(double measurement) {
  m_measurement = measurement;
  m_prevError = m_positionError;

  if (m_continuous) {
    double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
    m_positionError =
        frc::InputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
  } else {
    m_positionError = m_setpoint - m_measurement;
  }

  m_velocityError = (m_positionError - m_prevError) / m_period.value();

  if (m_Ki != 0) {
    m_totalError =
        std::clamp(m_totalError + m_positionError * m_period.value(),
                   m_minimumIntegral / m_Ki, m_maximumIntegral / m_Ki);
  }

  return m_Kp * m_positionError + m_Ki * m_totalError + m_Kd * m_velocityError;
}

double PIDController::Calculate(double measurement, double setpoint) {
  m_setpoint = setpoint;
  return Calculate(measurement);
}

void PIDController::Reset() {
  m_positionError = 0;
  m_prevError = 0;
  m_totalError = 0;
  m_velocityError = 0;
}

void PIDController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.AddDoubleProperty(
      "p", [this] { return GetP(); }, [this](double value) { SetP(value); });
  builder.AddDoubleProperty(
      "i", [this] { return GetI(); }, [this](double value) { SetI(value); });
  builder.AddDoubleProperty(
      "d", [this] { return GetD(); }, [this](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint(); },
      [this](double value) { SetSetpoint(value); });
}
