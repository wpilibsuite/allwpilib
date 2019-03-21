/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/AnalogAccelerometer.h"

using namespace frc::experimental;

AnalogAccelerometer::AnalogAccelerometer(int channel) : m_impl(channel) {}

AnalogAccelerometer::AnalogAccelerometer(AnalogInput& channel)
    : m_impl(&channel) {}

double AnalogAccelerometer::GetAcceleration() const {
  return m_impl.GetAcceleration();
}

void AnalogAccelerometer::SetSensitivity(double sensitivity) {
  m_impl.SetSensitivity(sensitivity);
}

void AnalogAccelerometer::SetZero(double zero) { m_impl.SetZero(zero); }

double AnalogAccelerometer::GetMeasurement() const { return GetAcceleration(); }

void AnalogAccelerometer::InitSendable(SendableBuilder& builder) {
  m_impl.InitSendable(builder);
}
