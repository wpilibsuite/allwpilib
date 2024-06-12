// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogEncoder.h"

#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/AnalogInput.h"
#include "frc/Counter.h"
#include "frc/Errors.h"
#include "frc/RobotController.h"

using namespace frc;

AnalogEncoder::AnalogEncoder(int channel)
    : AnalogEncoder(std::make_shared<AnalogInput>(channel)) {}

AnalogEncoder::AnalogEncoder(AnalogInput& analogInput)
    : m_analogInput{&analogInput, wpi::NullDeleter<AnalogInput>{}},
      m_analogTrigger{m_analogInput.get()},
      m_counter{} {
  Init();
}

AnalogEncoder::AnalogEncoder(AnalogInput* analogInput)
    : m_analogInput{analogInput, wpi::NullDeleter<AnalogInput>{}},
      m_analogTrigger{m_analogInput.get()},
      m_counter{} {
  Init();
}

AnalogEncoder::AnalogEncoder(std::shared_ptr<AnalogInput> analogInput)
    : m_analogInput{std::move(analogInput)},
      m_analogTrigger{m_analogInput.get()},
      m_counter{} {
  Init();
}

void AnalogEncoder::Init() {
  m_simDevice = hal::SimDevice{"AnalogEncoder", m_analogInput->GetChannel()};

  if (m_simDevice) {
    m_simPosition = m_simDevice.CreateDouble("Position", false, 0.0);
    m_simAbsolutePosition =
        m_simDevice.CreateDouble("absPosition", hal::SimDevice::kInput, 0.0);
  }

  m_analogTrigger.SetLimitsVoltage(1.25, 3.75);
  m_counter.SetUpSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));

  wpi::SendableRegistry::AddLW(this, "DutyCycle Encoder",
                               m_analogInput->GetChannel());
}

static bool DoubleEquals(double a, double b) {
  constexpr double epsilon = 0.00001;
  return std::abs(a - b) < epsilon;
}

units::turn_t AnalogEncoder::Get() const {
  if (m_simPosition) {
    return units::turn_t{m_simPosition.Get()};
  }

  // As the values are not atomic, keep trying until we get 2 reads of the same
  // value If we don't within 10 attempts, error
  for (int i = 0; i < 10; i++) {
    auto counter = m_counter.Get();
    auto pos = m_analogInput->GetVoltage();
    auto counter2 = m_counter.Get();
    auto pos2 = m_analogInput->GetVoltage();
    if (counter == counter2 && DoubleEquals(pos, pos2)) {
      pos = pos / frc::RobotController::GetVoltage5V();
      units::turn_t turns{counter + pos - m_positionOffset};
      m_lastPosition = turns;
      return turns;
    }
  }

  FRC_ReportError(
      warn::Warning,
      "Failed to read Analog Encoder. Potential Speed Overrun. Returning last "
      "value");
  return m_lastPosition;
}

double AnalogEncoder::GetAbsolutePosition() const {
  if (m_simAbsolutePosition) {
    return m_simAbsolutePosition.Get();
  }

  return m_analogInput->GetVoltage() / frc::RobotController::GetVoltage5V();
}

double AnalogEncoder::GetPositionOffset() const {
  return m_positionOffset;
}

void AnalogEncoder::SetPositionOffset(double offset) {
  m_positionOffset = std::clamp(offset, 0.0, 1.0);
}

void AnalogEncoder::SetDistancePerRotation(double distancePerRotation) {
  m_distancePerRotation = distancePerRotation;
}

double AnalogEncoder::GetDistancePerRotation() const {
  return m_distancePerRotation;
}

double AnalogEncoder::GetDistance() const {
  return Get().value() * GetDistancePerRotation();
}

void AnalogEncoder::Reset() {
  m_counter.Reset();
  m_positionOffset =
      m_analogInput->GetVoltage() / frc::RobotController::GetVoltage5V();
}

int AnalogEncoder::GetChannel() const {
  return m_analogInput->GetChannel();
}

void AnalogEncoder::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("AbsoluteEncoder");
  builder.AddDoubleProperty(
      "Distance", [this] { return this->GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance Per Rotation",
      [this] { return this->GetDistancePerRotation(); }, nullptr);
}
