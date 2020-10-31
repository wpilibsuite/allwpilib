/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>

#include "frc/controller/ControllerUtil.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc2;

PIDController::PIDController(double Kp, double Ki, double Kd,
                             units::second_t period)
    : m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController2, instances);
  frc::SendableRegistry::GetInstance().Add(this, "PIDController", instances);
}

void PIDController::SetPID(double Kp, double Ki, double Kd) {
  m_Kp = Kp;
  m_Ki = Ki;
  m_Kd = Kd;
}

void PIDController::SetP(double Kp) { m_Kp = Kp; }

void PIDController::SetI(double Ki) { m_Ki = Ki; }

void PIDController::SetD(double Kd) { m_Kd = Kd; }

double PIDController::GetP() const { return m_Kp; }

double PIDController::GetI() const { return m_Ki; }

double PIDController::GetD() const { return m_Kd; }

units::second_t PIDController::GetPeriod() const {
  return units::second_t(m_period);
}

void PIDController::SetSetpoint(double setpoint) { m_setpoint = setpoint; }

double PIDController::GetSetpoint() const { return m_setpoint; }

bool PIDController::AtSetpoint() const {
  double positionError;
  if (m_continuous) {
    positionError = frc::GetModulusError<double>(
        m_setpoint, m_measurement, m_minimumInput, m_maximumInput);
  } else {
    positionError = m_setpoint - m_measurement;
  }

  double velocityError = (positionError - m_prevError) / m_period.to<double>();

  return std::abs(positionError) < m_positionTolerance &&
         std::abs(velocityError) < m_velocityTolerance;
}

void PIDController::EnableContinuousInput(double minimumInput,
                                          double maximumInput) {
  m_continuous = true;
  m_minimumInput = minimumInput;
  m_maximumInput = maximumInput;
}

void PIDController::DisableContinuousInput() { m_continuous = false; }

bool PIDController::IsContinuousInputEnabled() const { return m_continuous; }

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

double PIDController::GetPositionError() const { return m_positionError; }

double PIDController::GetVelocityError() const { return m_velocityError; }

double PIDController::Calculate(double measurement) {
  m_measurement = measurement;
  m_prevError = m_positionError;

  if (m_continuous) {
    m_positionError = frc::GetModulusError<double>(
        m_setpoint, measurement, m_minimumInput, m_maximumInput);
  } else {
    m_positionError = m_setpoint - measurement;
  }

  m_velocityError = (m_positionError - m_prevError) / m_period.to<double>();

  if (m_Ki != 0) {
    m_totalError =
        std::clamp(m_totalError + m_positionError * m_period.to<double>(),
                   m_minimumIntegral / m_Ki, m_maximumIntegral / m_Ki);
  }

  return m_Kp * m_positionError + m_Ki * m_totalError + m_Kd * m_velocityError;
}

double PIDController::Calculate(double measurement, double setpoint) {
  // Set setpoint to provided value
  SetSetpoint(setpoint);
  return Calculate(measurement);
}

void PIDController::Reset() {
  m_prevError = 0;
  m_totalError = 0;
}

void PIDController::InitSendable(frc::SendableBuilder& builder) {
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
