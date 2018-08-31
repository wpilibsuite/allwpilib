/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/controller/PIDController.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc::experimental;

template <class T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
  return std::max(low, std::min(value, high));
}

PIDController::PIDController(double Kp, double Ki, double Kd,
                             std::function<double()> measurementSource,
                             double period)
    : PIDController(Kp, Ki, Kd, [] { return 0.0; }, measurementSource, period) {
}

PIDController::PIDController(double Kp, double Ki, double Kd,
                             MeasurementSource& measurementSource,
                             double period)
    : PIDController(Kp, Ki, Kd, [] { return 0.0; },
                    [&] { return measurementSource.GetMeasurement(); },
                    period) {}

PIDController::PIDController(double Kp, double Ki, double Kd,
                             std::function<double()> feedforward,
                             std::function<double()> measurementSource,
                             double period)
    : Controller(period), SendableBase(false) {
  m_Kp = Kp;
  m_Ki = Ki;
  m_Kd = Kd;
  m_feedforward = feedforward;
  m_measurementSource = measurementSource;

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController, instances);
  SetName("PIDController", instances);
}

PIDController::PIDController(double Kp, double Ki, double Kd,
                             std::function<double()> feedforward,
                             MeasurementSource& measurementSource,
                             double period)
    : PIDController(Kp, Ki, Kd, feedforward,
                    [&] { return measurementSource.GetMeasurement(); },
                    period) {}

void PIDController::SetPID(double Kp, double Ki, double Kd) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_Kp = Kp;
  m_Ki = Ki;
  m_Kd = Kd;
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

double PIDController::GetOutput() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_output;
}

void PIDController::SetReference(double reference) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);

  if (m_maximumInput > m_minimumInput) {
    m_reference = clamp(reference, m_minimumInput, m_maximumInput);
  } else {
    m_reference = reference;
  }
}

double PIDController::GetReference() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_reference;
}

bool PIDController::AtReference() const {
  double error = GetError();

  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  double deltaError = (error - m_prevError) / GetPeriod();
  if (m_toleranceType == Tolerance::kPercent) {
    return std::abs(error) < m_tolerance / 100 * m_inputRange &&
           std::abs(deltaError) < m_deltaTolerance / 100 * m_inputRange;
  } else {
    return std::abs(error) < m_tolerance &&
           std::abs(deltaError) < m_deltaTolerance;
  }
}

void PIDController::SetContinuous(bool continuous) {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_continuous = continuous;
}

void PIDController::SetInputRange(double minimumInput, double maximumInput) {
  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
    m_inputRange = maximumInput - minimumInput;
  }

  SetReference(GetReference());
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
  return GetContinuousError(m_reference - m_measurementSource());
}

/**
 * Returns the change in error per second of the PIDController.
 *
 * @return The change in error per second.
 */
double PIDController::GetDeltaError() const {
  double error = GetError();

  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return (error - m_prevError) / GetPeriod();
}

double PIDController::Update() {
  // Storage for function inputs
  double Kp;
  double Ki;
  double Kd;
  double feedforward = m_feedforward();
  double measurement = m_measurementSource();
  double minimumOutput;
  double maximumOutput;

  // Storage for function input-outputs
  double prevError;
  double error;
  double totalError;

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);

    Kp = m_Kp;
    Ki = m_Ki;
    Kd = m_Kd;
    minimumOutput = m_minimumOutput;
    maximumOutput = m_maximumOutput;

    prevError = m_prevError;
    error = GetContinuousError(m_reference - measurement);
    totalError = m_totalError;
  }

  if (Ki != 0) {
    totalError = clamp(totalError + error * GetPeriod(), minimumOutput / Ki,
                       maximumOutput / Ki);
  }

  double output =
      clamp(Kp * error + Ki * totalError +
                Kd * (error - prevError) / GetPeriod() + feedforward,
            minimumOutput, maximumOutput);

  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    m_prevError = error;
    m_totalError = totalError;
    m_output = output;
  }

  return output;
}

void PIDController::Reset() {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  m_prevError = 0;
  m_totalError = 0;
  m_output = 0;
}

void PIDController::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.SetSafeState([=]() { Reset(); });
  builder.AddDoubleProperty("Kp", [=] { return GetP(); },
                            [=](double value) { SetP(value); });
  builder.AddDoubleProperty("Ki", [=] { return GetI(); },
                            [=](double value) { SetI(value); });
  builder.AddDoubleProperty("Kd", [=] { return GetD(); },
                            [=](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "feedforward", [=] { return m_feedforward(); },
      [=](double value) { m_feedforward = [=] { return value; }; });
  builder.AddDoubleProperty("reference", [=] { return GetReference(); },
                            [=](double value) { SetReference(value); });
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
