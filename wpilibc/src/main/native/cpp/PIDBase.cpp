/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PIDBase.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>

#include "frc/PIDOutput.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

template <class T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
  return std::max(low, std::min(value, high));
}

PIDBase::PIDBase(double Kp, double Ki, double Kd, PIDSource& source,
                 PIDOutput& output)
    : PIDBase(Kp, Ki, Kd, 0.0, source, output) {}

PIDBase::PIDBase(double Kp, double Ki, double Kd, double Kf, PIDSource& source,
                 PIDOutput& output) {
  m_P = Kp;
  m_I = Ki;
  m_D = Kd;
  m_F = Kf;

  m_pidInput = &source;
  m_filter = LinearFilter<double>::MovingAverage(1);

  m_pidOutput = &output;

  m_setpointTimer.Start();

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController, instances);
  SendableRegistry::GetInstance().Add(this, "PIDController", instances);
}

double PIDBase::Get() const {
  std::scoped_lock lock(m_thisMutex);
  return m_result;
}

void PIDBase::SetContinuous(bool continuous) {
  std::scoped_lock lock(m_thisMutex);
  m_continuous = continuous;
}

void PIDBase::SetInputRange(double minimumInput, double maximumInput) {
  {
    std::scoped_lock lock(m_thisMutex);
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
    m_inputRange = maximumInput - minimumInput;
  }

  SetSetpoint(m_setpoint);
}

void PIDBase::SetOutputRange(double minimumOutput, double maximumOutput) {
  std::scoped_lock lock(m_thisMutex);
  m_minimumOutput = minimumOutput;
  m_maximumOutput = maximumOutput;
}

void PIDBase::SetPID(double p, double i, double d) {
  {
    std::scoped_lock lock(m_thisMutex);
    m_P = p;
    m_I = i;
    m_D = d;
  }
}

void PIDBase::SetPID(double p, double i, double d, double f) {
  std::scoped_lock lock(m_thisMutex);
  m_P = p;
  m_I = i;
  m_D = d;
  m_F = f;
}

void PIDBase::SetP(double p) {
  std::scoped_lock lock(m_thisMutex);
  m_P = p;
}

void PIDBase::SetI(double i) {
  std::scoped_lock lock(m_thisMutex);
  m_I = i;
}

void PIDBase::SetD(double d) {
  std::scoped_lock lock(m_thisMutex);
  m_D = d;
}

void PIDBase::SetF(double f) {
  std::scoped_lock lock(m_thisMutex);
  m_F = f;
}

double PIDBase::GetP() const {
  std::scoped_lock lock(m_thisMutex);
  return m_P;
}

double PIDBase::GetI() const {
  std::scoped_lock lock(m_thisMutex);
  return m_I;
}

double PIDBase::GetD() const {
  std::scoped_lock lock(m_thisMutex);
  return m_D;
}

double PIDBase::GetF() const {
  std::scoped_lock lock(m_thisMutex);
  return m_F;
}

void PIDBase::SetSetpoint(double setpoint) {
  {
    std::scoped_lock lock(m_thisMutex);

    if (m_maximumInput > m_minimumInput) {
      if (setpoint > m_maximumInput)
        m_setpoint = m_maximumInput;
      else if (setpoint < m_minimumInput)
        m_setpoint = m_minimumInput;
      else
        m_setpoint = setpoint;
    } else {
      m_setpoint = setpoint;
    }
  }
}

double PIDBase::GetSetpoint() const {
  std::scoped_lock lock(m_thisMutex);
  return m_setpoint;
}

double PIDBase::GetDeltaSetpoint() const {
  std::scoped_lock lock(m_thisMutex);
  return (m_setpoint - m_prevSetpoint) / m_setpointTimer.Get();
}

double PIDBase::GetError() const {
  double setpoint = GetSetpoint();
  {
    std::scoped_lock lock(m_thisMutex);
    return GetContinuousError(setpoint - m_pidInput->PIDGet());
  }
}

double PIDBase::GetAvgError() const { return GetError(); }

void PIDBase::SetPIDSourceType(PIDSourceType pidSource) {
  m_pidInput->SetPIDSourceType(pidSource);
}

PIDSourceType PIDBase::GetPIDSourceType() const {
  return m_pidInput->GetPIDSourceType();
}

void PIDBase::SetTolerance(double percent) {
  std::scoped_lock lock(m_thisMutex);
  m_toleranceType = kPercentTolerance;
  m_tolerance = percent;
}

void PIDBase::SetAbsoluteTolerance(double absTolerance) {
  std::scoped_lock lock(m_thisMutex);
  m_toleranceType = kAbsoluteTolerance;
  m_tolerance = absTolerance;
}

void PIDBase::SetPercentTolerance(double percent) {
  std::scoped_lock lock(m_thisMutex);
  m_toleranceType = kPercentTolerance;
  m_tolerance = percent;
}

void PIDBase::SetToleranceBuffer(int bufLength) {
  std::scoped_lock lock(m_thisMutex);
  m_filter = LinearFilter<double>::MovingAverage(bufLength);
}

bool PIDBase::OnTarget() const {
  double error = GetError();

  std::scoped_lock lock(m_thisMutex);
  switch (m_toleranceType) {
    case kPercentTolerance:
      return std::fabs(error) < m_tolerance / 100 * m_inputRange;
      break;
    case kAbsoluteTolerance:
      return std::fabs(error) < m_tolerance;
      break;
    case kNoTolerance:
      // TODO: this case needs an error
      return false;
  }
  return false;
}

void PIDBase::Reset() {
  std::scoped_lock lock(m_thisMutex);
  m_prevError = 0;
  m_totalError = 0;
  m_result = 0;
}

void PIDBase::PIDWrite(double output) { SetSetpoint(output); }

void PIDBase::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.SetSafeState([=]() { Reset(); });
  builder.AddDoubleProperty(
      "p", [=]() { return GetP(); }, [=](double value) { SetP(value); });
  builder.AddDoubleProperty(
      "i", [=]() { return GetI(); }, [=](double value) { SetI(value); });
  builder.AddDoubleProperty(
      "d", [=]() { return GetD(); }, [=](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "f", [=]() { return GetF(); }, [=](double value) { SetF(value); });
  builder.AddDoubleProperty(
      "setpoint", [=]() { return GetSetpoint(); },
      [=](double value) { SetSetpoint(value); });
}

void PIDBase::Calculate() {
  if (m_pidInput == nullptr || m_pidOutput == nullptr) return;

  bool enabled;
  {
    std::scoped_lock lock(m_thisMutex);
    enabled = m_enabled;
  }

  if (enabled) {
    double input;

    // Storage for function inputs
    PIDSourceType pidSourceType;
    double P;
    double I;
    double D;
    double feedForward = CalculateFeedForward();
    double minimumOutput;
    double maximumOutput;

    // Storage for function input-outputs
    double prevError;
    double error;
    double totalError;

    {
      std::scoped_lock lock(m_thisMutex);

      input = m_filter.Calculate(m_pidInput->PIDGet());

      pidSourceType = m_pidInput->GetPIDSourceType();
      P = m_P;
      I = m_I;
      D = m_D;
      minimumOutput = m_minimumOutput;
      maximumOutput = m_maximumOutput;

      prevError = m_prevError;
      error = GetContinuousError(m_setpoint - input);
      totalError = m_totalError;
    }

    // Storage for function outputs
    double result;

    if (pidSourceType == PIDSourceType::kRate) {
      if (P != 0) {
        totalError =
            clamp(totalError + error, minimumOutput / P, maximumOutput / P);
      }

      result = D * error + P * totalError + feedForward;
    } else {
      if (I != 0) {
        totalError =
            clamp(totalError + error, minimumOutput / I, maximumOutput / I);
      }

      result =
          P * error + I * totalError + D * (error - prevError) + feedForward;
    }

    result = clamp(result, minimumOutput, maximumOutput);

    {
      // Ensures m_enabled check and PIDWrite() call occur atomically
      std::scoped_lock pidWriteLock(m_pidWriteMutex);
      std::unique_lock mainLock(m_thisMutex);
      if (m_enabled) {
        // Don't block other PIDBase operations on PIDWrite()
        mainLock.unlock();

        m_pidOutput->PIDWrite(result);
      }
    }

    std::scoped_lock lock(m_thisMutex);
    m_prevError = m_error;
    m_error = error;
    m_totalError = totalError;
    m_result = result;
  }
}

double PIDBase::CalculateFeedForward() {
  if (m_pidInput->GetPIDSourceType() == PIDSourceType::kRate) {
    return m_F * GetSetpoint();
  } else {
    double temp = m_F * GetDeltaSetpoint();
    m_prevSetpoint = m_setpoint;
    m_setpointTimer.Reset();
    return temp;
  }
}

double PIDBase::GetContinuousError(double error) const {
  if (m_continuous && m_inputRange != 0) {
    error = std::fmod(error, m_inputRange);
    if (std::fabs(error) > m_inputRange / 2) {
      if (error > 0) {
        return error - m_inputRange;
      } else {
        return error + m_inputRange;
      }
    }
  }

  return error;
}
