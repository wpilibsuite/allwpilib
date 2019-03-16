/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/AnalogInput.h"

#include <utility>

#include <hal/AnalogAccumulator.h>
#include <hal/AnalogInput.h>
#include <hal/HAL.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/Timer.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc::experimental;

AnalogInput::AnalogInput(int channel) : m_impl(channel) {}

int AnalogInput::GetValue() const { return m_impl.GetValue(); }

int AnalogInput::GetAverageValue() const { return m_impl.GetAverageValue(); }

double AnalogInput::GetVoltage() const { return m_impl.GetVoltage(); }

double AnalogInput::GetAverageVoltage() const {
  return m_impl.GetAverageVoltage();
}

int AnalogInput::GetChannel() const { return m_impl.GetChannel(); }

void AnalogInput::SetAverageBits(int bits) { m_impl.SetAverageBits(bits); }

int AnalogInput::GetAverageBits() const { return m_impl.GetAverageBits(); }

void AnalogInput::SetOversampleBits(int bits) {
  m_impl.SetOversampleBits(bits);
}

int AnalogInput::GetOversampleBits() const {
  return m_impl.GetOversampleBits();
}

int AnalogInput::GetLSBWeight() const { return m_impl.GetLSBWeight(); }

int AnalogInput::GetOffset() const { return m_impl.GetOffset(); }

bool AnalogInput::IsAccumulatorChannel() const {
  return m_impl.IsAccumulatorChannel();
}

void AnalogInput::InitAccumulator() { m_impl.InitAccumulator(); }

void AnalogInput::SetAccumulatorInitialValue(int64_t initialValue) {
  m_impl.SetAccumulatorInitialValue(initialValue);
}

void AnalogInput::ResetAccumulator() { m_impl.ResetAccumulator(); }

void AnalogInput::SetAccumulatorCenter(int center) {
  m_impl.SetAccumulatorCenter(center);
}

void AnalogInput::SetAccumulatorDeadband(int deadband) {
  m_impl.SetAccumulatorDeadband(deadband);
}

int64_t AnalogInput::GetAccumulatorValue() const {
  return m_impl.GetAccumulatorValue();
}

int64_t AnalogInput::GetAccumulatorCount() const {
  return m_impl.GetAccumulatorCount();
}

void AnalogInput::GetAccumulatorOutput(int64_t& value, int64_t& count) const {
  return m_impl.GetAccumulatorOutput(value, count);
}

void AnalogInput::SetSampleRate(double samplesPerSecond) {
  frc::AnalogInput::SetSampleRate(samplesPerSecond);
}

double AnalogInput::GetSampleRate() {
  return frc::AnalogInput::GetSampleRate();
}

double AnalogInput::GetMeasurement() const { return GetAverageVoltage(); }

void AnalogInput::InitSendable(SendableBuilder& builder) {
  m_impl.InitSendable(builder);
}
