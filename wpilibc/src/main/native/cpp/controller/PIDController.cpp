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

using namespace frc::experimental;

template <class T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
  return std::max(low, std::min(value, high));
}

PIDController::PIDController(double Kp, double Ki, double Kd, double period)
    : SendableBase(false), m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController, instances);
  SetName("PIDController", instances);
}

void PIDController::SetP(double Kp) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_Kp = Kp;
}

void PIDController::SetI(double Ki) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_Ki = Ki;
}

void PIDController::SetD(double Kd) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_Kd = Kd;
}

double PIDController::GetP() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_Kp;
}

double PIDController::GetI() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_Ki;
}

double PIDController::GetD() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_Kd;
}

double PIDController::GetPeriod() const { return m_period; }

double PIDController::GetOutput() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_output;
}

void PIDController::SetSetpoint(double setpoint) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);

  if (m_maximumInput > m_minimumInput) {
    m_setpoint = clamp(setpoint, m_minimumInput, m_maximumInput);
  } else {
    m_setpoint = setpoint;
  }
}

double PIDController::GetSetpoint() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_setpoint;
}

bool PIDController::AtSetpoint(double tolerance, double deltaTolerance,
                               Tolerance toleranceType) const {
  double deltaError = GetDeltaError();

  std::lock_guard<wpi::mutex> lock(m_thisMutex);
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
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_continuous = continuous;
}

void PIDController::SetInputRange(double minimumInput, double maximumInput) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_minimumInput = minimumInput;
  m_maximumInput = maximumInput;
  m_inputRange = maximumInput - minimumInput;

  // Clamp setpoint to new input range
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = clamp(m_setpoint, m_minimumInput, m_maximumInput);
  }
}

void PIDController::SetOutputRange(double minimumOutput, double maximumOutput) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_minimumOutput = minimumOutput;
  m_maximumOutput = maximumOutput;
}

void PIDController::SetAbsoluteTolerance(double tolerance,
                                         double deltaTolerance) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_toleranceType = Tolerance::kAbsolute;
  m_tolerance = tolerance;
  m_deltaTolerance = deltaTolerance;
}

void PIDController::SetPercentTolerance(double tolerance,
                                        double deltaTolerance) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_toleranceType = Tolerance::kPercent;
  m_tolerance = tolerance;
  m_deltaTolerance = deltaTolerance;
}

double PIDController::GetError() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return GetContinuousError(m_currError);
}

/**
 * Returns the change in error per second of the PIDController.
 *
 * @return The change in error per second.
 */
double PIDController::GetDeltaError() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return (m_currError - m_prevError) / GetPeriod();
}

double PIDController::Calculate(double measurement) {
  // Storage for function inputs
  double Kp;
  double Ki;
  double Kd;
  double minimumOutput;
  double maximumOutput;

  // Storage for function input-outputs
  double totalError;

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);

    Kp = m_Kp;
    Ki = m_Ki;
    Kd = m_Kd;
    minimumOutput = m_minimumOutput;
    maximumOutput = m_maximumOutput;

    m_prevError = m_currError;
    m_currError = GetContinuousError(m_setpoint - measurement);
    totalError = m_totalError;
  }

  if (Ki != 0) {
    totalError = clamp(totalError + m_currError * GetPeriod(),
                       minimumOutput / Ki, maximumOutput / Ki);
  }

  double output = clamp(Kp * m_currError + Ki * totalError +
                            Kd * (m_currError - m_prevError) / GetPeriod(),
                        minimumOutput, maximumOutput);

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    m_totalError = totalError;
    m_output = output;
  }

  return output;
}

double PIDController::Calculate(double measurement, double setpoint) {
  SetSetpoint(setpoint);
  return Calculate(measurement);
}

void PIDController::Reset() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
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
