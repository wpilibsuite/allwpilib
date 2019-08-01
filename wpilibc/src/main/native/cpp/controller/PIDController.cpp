/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/PIDController.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc2;

PIDController::PIDController(double Kp, double Ki, double Kd, double period)
    : frc::SendableBase(false), m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController, instances);
  SetName("PIDController", instances);
}

void PIDController::SetP(double Kp) { m_Kp = Kp; }

void PIDController::SetI(double Ki) { m_Ki = Ki; }

void PIDController::SetD(double Kd) { m_Kd = Kd; }

double PIDController::GetP() const { return m_Kp; }

double PIDController::GetI() const { return m_Ki; }

double PIDController::GetD() const { return m_Kd; }

double PIDController::GetPeriod() const { return m_period; }

double PIDController::GetOutput() const { return m_output; }

void PIDController::SetSetpoint(double setpoint) {
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = std::clamp(setpoint, m_minimumInput, m_maximumInput);
  } else {
    m_setpoint = setpoint;
  }
}

double PIDController::GetSetpoint() const { return m_setpoint; }

bool PIDController::AtSetpoint(double tolerance, double deltaTolerance,
                               Tolerance toleranceType) const {
  double deltaError = GetDeltaError();

  if (toleranceType == Tolerance::kPercent) {
    return std::abs(m_currError) < tolerance / 100 * m_inputRange &&
           std::abs(deltaError) < deltaTolerance / 100 * m_inputRange;
  } else {
    return std::abs(m_currError) < tolerance &&
           std::abs(deltaError) < deltaTolerance;
  }
}

bool PIDController::AtSetpoint() const {
  return AtSetpoint(m_tolerance, m_deltaTolerance, m_toleranceType);
}

void PIDController::SetContinuous(bool continuous) {
  m_continuous = continuous;
}

void PIDController::SetInputRange(double minimumInput, double maximumInput) {
  m_minimumInput = minimumInput;
  m_maximumInput = maximumInput;
  m_inputRange = maximumInput - minimumInput;

  // Clamp setpoint to new input range
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = std::clamp(m_setpoint, m_minimumInput, m_maximumInput);
  }
}

void PIDController::SetOutputRange(double minimumOutput, double maximumOutput) {
  m_minimumOutput = minimumOutput;
  m_maximumOutput = maximumOutput;
}

void PIDController::SetAbsoluteTolerance(double tolerance,
                                         double deltaTolerance) {
  m_toleranceType = Tolerance::kAbsolute;
  m_tolerance = tolerance;
  m_deltaTolerance = deltaTolerance;
}

void PIDController::SetPercentTolerance(double tolerance,
                                        double deltaTolerance) {
  m_toleranceType = Tolerance::kPercent;
  m_tolerance = tolerance;
  m_deltaTolerance = deltaTolerance;
}

double PIDController::GetError() const {
  return GetContinuousError(m_currError);
}

/**
 * Returns the change in error per second of the PIDController.
 *
 * @return The change in error per second.
 */
double PIDController::GetDeltaError() const {
  return (m_currError - m_prevError) / GetPeriod();
}

double PIDController::Calculate(double measurement, double setpoint) {
  // Set setpoint to provided value
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = std::clamp(setpoint, m_minimumInput, m_maximumInput);
  } else {
    m_setpoint = setpoint;
  }

  return Calculate(measurement);
}

void PIDController::Reset() {
  m_prevError = 0;
  m_totalError = 0;
  m_output = 0;
}

void PIDController::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.AddDoubleProperty("p", [this] { return GetP(); },
                            [this](double value) { SetP(value); });
  builder.AddDoubleProperty("i", [this] { return GetI(); },
                            [this](double value) { SetI(value); });
  builder.AddDoubleProperty("d", [this] { return GetD(); },
                            [this](double value) { SetD(value); });
  builder.AddDoubleProperty("setpoint", [this] { return GetSetpoint(); },
                            [this](double value) { SetSetpoint(value); });
}

double PIDController::GetContinuousError(double error) const {
  if (m_continuous && m_inputRange > 0) {
    error = std::fmod(error, m_inputRange);
    if (std::abs(error) > m_inputRange / 2) {
      if (error > 0) {
        return error - m_inputRange;
      } else {
        return error + m_inputRange;
      }
    }
  }

  return error;
}

double PIDController::Calculate(double measurement) {
  m_prevError = m_currError;
  m_currError = GetContinuousError(m_setpoint - measurement);

  if (m_Ki != 0) {
    m_totalError = std::clamp(m_totalError + m_currError * GetPeriod(),
                              m_minimumOutput / m_Ki, m_maximumOutput / m_Ki);
  }

  m_output = std::clamp(m_Kp * m_currError + m_Ki * m_totalError +
                            m_Kd * (m_currError - m_prevError) / GetPeriod(),
                        m_minimumOutput, m_maximumOutput);

  return m_output;
}
