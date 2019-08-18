/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableBuilder.h"

namespace frc2 {

template <typename Unit>
PIDController<Unit>::PIDController(double Kp, double Ki, double Kd,
                                   units::second_t period)
    : frc::SendableBase(false), m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_period(period) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_PIDController, instances);
  SetName("PIDController", instances);
}

template <typename Unit>
void PIDController<Unit>::SetP(double Kp) {
  m_Kp = Kp;
}

template <typename Unit>
void PIDController<Unit>::SetI(double Ki) {
  m_Ki = Ki;
}

template <typename Unit>
void PIDController<Unit>::SetD(double Kd) {
  m_Kd = Kd;
}

template <typename Unit>
double PIDController<Unit>::GetP() const {
  return m_Kp;
}

template <typename Unit>
double PIDController<Unit>::GetI() const {
  return m_Ki;
}

template <typename Unit>
double PIDController<Unit>::GetD() const {
  return m_Kd;
}

template <typename Unit>
units::second_t PIDController<Unit>::GetPeriod() const {
  return units::second_t(m_period);
}

template <typename Unit>
void PIDController<Unit>::SetSetpoint(Unit setpoint) {
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = units::math::clamp(setpoint, m_minimumInput, m_maximumInput);
  } else {
    m_setpoint = setpoint;
  }
}

template <typename Unit>
Unit PIDController<Unit>::GetSetpoint() const {
  return m_setpoint;
}

template <typename Unit>
bool PIDController<Unit>::AtSetpoint() const {
  return units::math::abs(m_positionError) < m_positionTolerance &&
         units::math::abs(m_velocityError) < m_velocityTolerance;
}

template <typename Unit>
void PIDController<Unit>::EnableContinuousInput(Unit minimumInput,
                                                Unit maximumInput) {
  m_continuous = true;

  m_minimumInput = minimumInput;
  m_maximumInput = maximumInput;
  m_inputRange = maximumInput - minimumInput;

  // Clamp setpoint to new input range
  if (m_maximumInput > m_minimumInput) {
    m_setpoint = units::math::clamp(m_setpoint, m_minimumInput, m_maximumInput);
  }
}

template <typename Unit>
void PIDController<Unit>::DisableContinuousInput() {
  m_continuous = false;
}

template <typename Unit>
void PIDController<Unit>::SetOutputRange(double minimumOutput,
                                         double maximumOutput) {
  m_minimumOutput = minimumOutput;
  m_maximumOutput = maximumOutput;
}

template <typename Unit>
void PIDController<Unit>::SetTolerance(Unit positionTolerance,
                                       UnitPerSec velocityTolerance) {
  m_positionTolerance = positionTolerance;
  m_velocityTolerance = velocityTolerance;
}

template <typename Unit>
Unit PIDController<Unit>::GetPositionError() const {
  return GetContinuousPositionError(m_positionError);
}

template <typename Unit>
typename PIDController<Unit>::UnitPerSec PIDController<Unit>::GetVelocityError()
    const {
  return m_velocityError;
}

template <typename Unit>
double PIDController<Unit>::Calculate(Unit measurement) {
  m_prevError = m_positionError;
  m_positionError = GetContinuousPositionError(m_setpoint - measurement);
  m_velocityError = (m_positionError - m_prevError) / m_period;

  if (m_Ki != 0) {
    m_totalError =
        std::clamp(m_totalError + m_positionError.template to<double>() *
                                      m_period.template to<double>(),
                   m_minimumOutput / m_Ki, m_maximumOutput / m_Ki);
  }

  return std::clamp(m_Kp * m_positionError.template to<double>() +
                        m_Ki * m_totalError +
                        m_Kd * m_velocityError.template to<double>(),
                    m_minimumOutput, m_maximumOutput);
}

template <typename Unit>
double PIDController<Unit>::Calculate(Unit measurement, Unit setpoint) {
  // Set setpoint to provided value
  SetSetpoint(setpoint);
  return Calculate(measurement);
}

template <typename Unit>
void PIDController<Unit>::Reset() {
  m_prevError = 0;
  m_totalError = 0;
}

template <typename Unit>
void PIDController<Unit>::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.AddDoubleProperty("p", [this] { return GetP(); },
                            [this](double value) { SetP(value); });
  builder.AddDoubleProperty("i", [this] { return GetI(); },
                            [this](double value) { SetI(value); });
  builder.AddDoubleProperty("d", [this] { return GetD(); },
                            [this](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint().template to<double>(); },
      [this](double value) { SetSetpoint(Unit{value}); });
}

template <typename Unit>
Unit PIDController<Unit>::GetContinuousPositionError(Unit error) const {
  if (m_continuous && m_inputRange > Unit{0.0}) {
    error = units::math::fmod(error, m_inputRange);
    if (units::math::abs(error) > m_inputRange / 2) {
      if (error > Unit{0.0}) {
        return error - m_inputRange;
      } else {
        return error + m_inputRange;
      }
    }
  }

  return error;
}

}  // namespace frc2
