// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogEncoder.h"

#include <utility>

#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/AnalogInput.h"
#include "frc/MathUtil.h"
#include "frc/RobotController.h"

using namespace frc;

AnalogEncoder::~AnalogEncoder() {}

AnalogEncoder::AnalogEncoder(int channel)
    : AnalogEncoder(std::make_shared<AnalogInput>(channel)) {}

AnalogEncoder::AnalogEncoder(AnalogInput& analogInput)
    : m_analogInput{&analogInput, wpi::NullDeleter<AnalogInput>{}} {
  Init(1.0, 0.0);
}

AnalogEncoder::AnalogEncoder(AnalogInput* analogInput)
    : m_analogInput{analogInput, wpi::NullDeleter<AnalogInput>{}} {
  Init(1.0, 0.0);
}

AnalogEncoder::AnalogEncoder(std::shared_ptr<AnalogInput> analogInput)
    : m_analogInput{std::move(analogInput)} {
  Init(1.0, 0.0);
}

AnalogEncoder::AnalogEncoder(int channel, double fullRange, double expectedZero)
    : AnalogEncoder(std::make_shared<AnalogInput>(channel), fullRange,
                    expectedZero) {}

AnalogEncoder::AnalogEncoder(AnalogInput& analogInput, double fullRange,
                             double expectedZero)
    : m_analogInput{&analogInput, wpi::NullDeleter<AnalogInput>{}} {
  Init(fullRange, expectedZero);
}

AnalogEncoder::AnalogEncoder(AnalogInput* analogInput, double fullRange,
                             double expectedZero)
    : m_analogInput{analogInput, wpi::NullDeleter<AnalogInput>{}} {
  Init(fullRange, expectedZero);
}

AnalogEncoder::AnalogEncoder(std::shared_ptr<AnalogInput> analogInput,
                             double fullRange, double expectedZero)
    : m_analogInput{std::move(analogInput)} {
  Init(fullRange, expectedZero);
}

void AnalogEncoder::Init(double fullRange, double expectedZero) {
  m_simDevice = hal::SimDevice{"AnalogEncoder", m_analogInput->GetChannel()};

  if (m_simDevice) {
    m_simPosition = m_simDevice.CreateDouble("Position", false, 0.0);
  }

  m_fullRange = fullRange;
  m_expectedZero = expectedZero;

  wpi::SendableRegistry::AddLW(this, "Analog Encoder",
                               m_analogInput->GetChannel());
}

double AnalogEncoder::Get() const {
  if (m_simPosition) {
    return m_simPosition.Get();
  }

  double analog = m_analogInput->GetVoltage();
  double pos = analog / RobotController::GetVoltage5V();

  // Map sensor range if range isn't full
  pos = MapSensorRange(pos);

  // Compute full range and offset
  pos = pos * m_fullRange - m_expectedZero;

  // Map from 0 - Full Range
  double result = InputModulus(pos, 0.0, m_fullRange);
  // Invert if necessary
  if (m_isInverted) {
    return m_fullRange - result;
  }
  return result;
}

void AnalogEncoder::SetVoltagePercentageRange(double min, double max) {
  m_sensorMin = std::clamp(min, 0.0, 1.0);
  m_sensorMax = std::clamp(max, 0.0, 1.0);
}

void AnalogEncoder::SetInverted(bool inverted) {
  m_isInverted = inverted;
}

int AnalogEncoder::GetChannel() const {
  return m_analogInput->GetChannel();
}

double AnalogEncoder::MapSensorRange(double pos) const {
  if (pos < m_sensorMin) {
    pos = m_sensorMin;
  }
  if (pos > m_sensorMax) {
    pos = m_sensorMax;
  }
  pos = (pos - m_sensorMin) / (m_sensorMax - m_sensorMin);
  return pos;
}

void AnalogEncoder::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("AbsoluteEncoder");
  builder.AddDoubleProperty(
      "Position", [this] { return this->Get(); }, nullptr);
}
