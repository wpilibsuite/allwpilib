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

using namespace frc;

PIDController::PIDController(double Kp, double Ki, double Kd,
                             units::second_t period)
    : m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  bool invalidGains = false;
  if (Kp < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Kp must be a non-negative number, got {}!", Kp);
    invalidGains = true;
  }
  if (Ki < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Ki must be a non-negative number, got {}!", Ki);
    invalidGains = true;
  }
  if (Kd < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Kd must be a non-negative number, got {}!", Kd);
    invalidGains = true;
  }
  if (invalidGains) {
    m_Kp = 0.0;
    m_Ki = 0.0;
    m_Kd = 0.0;
    wpi::math::MathSharedStore::ReportWarning("PID gains defaulted to 0.");
  }

  if (period <= 0_s) {
    wpi::math::MathSharedStore::ReportError(
        "Controller period must be a positive number, got {}!", period.value());
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

void PIDController::SetIZone(double iZone) {
  if (iZone < 0) {
    wpi::math::MathSharedStore::ReportError(
        "IZone must be a non-negative number, got {}!", iZone);
  }
  m_iZone = iZone;
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

double PIDController::GetIZone() const {
  return m_iZone;
}

units::second_t PIDController::GetPeriod() const {
  return m_period;
}

double PIDController::GetPositionTolerance() const {
  return m_errorTolerance;
}

double PIDController::GetVelocityTolerance() const {
  return m_errorDerivativeTolerance;
}

double PIDController::GetAccumulatedError() const {
  return m_totalError;
}

void PIDController::SetSetpoint(double setpoint) {
  m_setpoint = setpoint;
  m_haveSetpoint = true;

  if (m_continuous) {
    double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
    m_error = InputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
  } else {
    m_error = m_setpoint - m_measurement;
  }

  m_errorDerivative = (m_error - m_prevError) / m_period.value();
}

double PIDController::GetSetpoint() const {
  return m_setpoint;
}

bool PIDController::AtSetpoint() const {
  return m_haveMeasurement && m_haveSetpoint &&
         std::abs(m_error) < m_errorTolerance &&
         std::abs(m_errorDerivative) < m_errorDerivativeTolerance;
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

void PIDController::SetTolerance(double errorTolerance,
                                 double errorDerivativeTolerance) {
  m_errorTolerance = errorTolerance;
  m_errorDerivativeTolerance = errorDerivativeTolerance;
}

double PIDController::GetErrorTolerance() const {
  return m_errorTolerance;
}

double PIDController::GetErrorDerivativeTolerance() const {
  return m_errorDerivativeTolerance;
}

double PIDController::GetError() const {
  return m_error;
}

double PIDController::GetErrorDerivative() const {
  return m_errorDerivative;
}

double PIDController::GetPositionError() const {
  return m_error;
}

double PIDController::GetVelocityError() const {
  return m_errorDerivative;
}

double PIDController::Calculate(double measurement) {
  m_measurement = measurement;
  m_prevError = m_error;
  m_haveMeasurement = true;

  if (m_continuous) {
    double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
    m_error = InputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
  } else {
    m_error = m_setpoint - m_measurement;
  }

  m_errorDerivative = (m_error - m_prevError) / m_period.value();

  // If the absolute value of the position error is outside of IZone, reset the
  // total error
  if (std::abs(m_error) > m_iZone) {
    m_totalError = 0;
  } else if (m_Ki != 0) {
    m_totalError =
        std::clamp(m_totalError + m_error * m_period.value(),
                   m_minimumIntegral / m_Ki, m_maximumIntegral / m_Ki);
  }

  return m_Kp * m_error + m_Ki * m_totalError + m_Kd * m_errorDerivative;
}

double PIDController::Calculate(double measurement, double setpoint) {
  m_setpoint = setpoint;
  m_haveSetpoint = true;
  return Calculate(measurement);
}

void PIDController::Reset() {
  m_error = 0;
  m_prevError = 0;
  m_totalError = 0;
  m_errorDerivative = 0;
  m_haveMeasurement = false;
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
      "izone", [this] { return GetIZone(); },
      [this](double value) { SetIZone(value); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint(); },
      [this](double value) { SetSetpoint(value); });
  builder.AddDoubleProperty(
      "measurement", [this] { return m_measurement; }, nullptr);
  builder.AddDoubleProperty("error", [this] { return GetError(); }, nullptr);
  builder.AddDoubleProperty(
      "error derivative", [this] { return GetErrorDerivative(); }, nullptr);
  builder.AddDoubleProperty(
      "previous error", [this] { return m_prevError; }, nullptr);
  builder.AddDoubleProperty(
      "total error", [this] { return GetAccumulatedError(); }, nullptr);
}
