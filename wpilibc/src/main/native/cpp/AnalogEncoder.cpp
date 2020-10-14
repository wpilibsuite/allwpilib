/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogEncoder.h"

#include "frc/AnalogInput.h"
#include "frc/Base.h"
#include "frc/Counter.h"
#include "frc/DriverStation.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

AnalogEncoder::AnalogEncoder(AnalogInput& analogInput)
    : m_analogInput{&analogInput, NullDeleter<AnalogInput>{}},
      m_analogTrigger{m_analogInput.get()},
      m_counter{} {
  Init();
}

AnalogEncoder::AnalogEncoder(AnalogInput* analogInput)
    : m_analogInput{analogInput, NullDeleter<AnalogInput>{}},
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
  }

  m_analogTrigger.SetLimitsVoltage(1.25, 3.75);
  m_counter.SetUpSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));

  SendableRegistry::GetInstance().AddLW(this, "DutyCycle Encoder",
                                        m_analogInput->GetChannel());
}

units::turn_t AnalogEncoder::Get() const {
  if (m_simPosition) return units::turn_t{m_simPosition.Get()};

  // As the values are not atomic, keep trying until we get 2 reads of the same
  // value If we don't within 10 attempts, error
  for (int i = 0; i < 10; i++) {
    auto counter = m_counter.Get();
    auto pos = m_analogInput->GetVoltage();
    auto counter2 = m_counter.Get();
    auto pos2 = m_analogInput->GetVoltage();
    if (counter == counter2 && pos == pos2) {
      units::turn_t turns{counter + pos - m_positionOffset};
      m_lastPosition = turns;
      return turns;
    }
  }

  frc::DriverStation::GetInstance().ReportWarning(
      "Failed to read Analog Encoder. Potential Speed Overrun. Returning last "
      "value");
  return m_lastPosition;
}

double AnalogEncoder::GetPositionOffset() const { return m_positionOffset; }

void AnalogEncoder::SetDistancePerRotation(double distancePerRotation) {
  m_distancePerRotation = distancePerRotation;
}

double AnalogEncoder::GetDistancePerRotation() const {
  return m_distancePerRotation;
}

double AnalogEncoder::GetDistance() const {
  return Get().to<double>() * GetDistancePerRotation();
}

void AnalogEncoder::Reset() {
  m_counter.Reset();
  m_positionOffset = m_analogInput->GetVoltage();
}

int AnalogEncoder::GetChannel() const { return m_analogInput->GetChannel(); }

void AnalogEncoder::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("AbsoluteEncoder");
  builder.AddDoubleProperty(
      "Distance", [this] { return this->GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance Per Rotation",
      [this] { return this->GetDistancePerRotation(); }, nullptr);
}
