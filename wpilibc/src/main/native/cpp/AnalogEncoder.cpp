// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogEncoder.h"

#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/AnalogInput.h"
#include "frc/Counter.h"
#include "frc/Errors.h"
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

  if (m_rolloverCounter) {
    return GetWithRollovers();
  } else {
    double analog = m_analogInput->GetVoltage();
    return GetWithoutRollovers(analog);
  }
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

void AnalogEncoder::ConfigureRolloverSupport(bool enable) {
  if (enable && !m_rolloverCounter) {
    m_rolloverTrigger = std::make_unique<AnalogTrigger>(m_analogInput.get());
    m_rolloverTrigger->SetLimitsVoltage(1.25, 3.75);
    m_rolloverCounter = std::make_unique<Counter>();
    m_rolloverCounter->SetUpSource(
        m_rolloverTrigger->CreateOutput(AnalogTriggerType::kRisingPulse));
    m_rolloverCounter->SetDownSource(
        m_rolloverTrigger->CreateOutput(AnalogTriggerType::kFallingPulse));
  } else if (!enable && m_rolloverCounter) {
    m_rolloverCounter = nullptr;
    m_rolloverTrigger = nullptr;
  }
}

void AnalogEncoder::ResetRollovers() {
  if (m_rolloverCounter) {
    m_rolloverCounter->Reset();
  }
}

double AnalogEncoder::GetWithoutRollovers(double analog) const {
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

static bool DoubleEquals(double a, double b) {
  constexpr double epsilon = 0.00001;
  return std::abs(a - b) < epsilon;
}

double AnalogEncoder::GetWithRollovers() const {
  // As the values are not atomic, keep trying until we get 2 reads of the same
  // value If we don't within 10 attempts, error
  for (int i = 0; i < 10; i++) {
    auto counter = m_rolloverCounter->Get();
    auto pos = m_analogInput->GetVoltage();
    auto counter2 = m_rolloverCounter->Get();
    auto pos2 = m_analogInput->GetVoltage();
    if (counter == counter2 && DoubleEquals(pos, pos2)) {
      pos = GetWithoutRollovers(pos);

      if (m_isInverted) {
        pos = pos - counter;
      } else {
        pos = pos + counter;
      }

      m_lastPosition = pos;
      return pos;
    }
  }

  FRC_ReportError(
      warn::Warning,
      "Failed to read Analog Encoder. Potential Speed Overrun. Returning last "
      "value");
  return m_lastPosition;
}
