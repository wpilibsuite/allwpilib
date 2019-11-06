/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogEncoder.h"

#include <frc/AnalogInput.h>

#include "frc/Counter.h"
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
  m_analogTrigger.SetLimitsVoltage(1.25, 3.75);
  m_counter.SetUpSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));

  SendableRegistry::GetInstance().AddLW(this, "DutyCycle Encoder",
                                        m_analogInput->GetChannel());
}

int AnalogEncoder::GetRotations() const { return m_counter.Get(); }

double AnalogEncoder::Get() const {
  return GetRotations() + GetPositionInRotation() - m_positionOffset;
}

double AnalogEncoder::GetPositionInRotation() const {
  return m_analogInput->GetVoltage() / 5.0;
}

double AnalogEncoder::GetPositionOffset() const { return m_positionOffset; }

void AnalogEncoder::SetDistancePerRotation(double distancePerRotation) {
  m_distancePerRotation = distancePerRotation;
}
double AnalogEncoder::GetDistancePerRotation() const {
  return m_distancePerRotation;
}
double AnalogEncoder::GetDistance() const {
  return Get() * GetDistancePerRotation();
}

void AnalogEncoder::Reset() {
  m_counter.Reset();
  m_positionOffset = GetPositionInRotation();
}

void AnalogEncoder::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("AbsoluteEncoder");
  builder.AddDoubleProperty("Distance", [this] { return this->GetDistance(); },
                            nullptr);
  builder.AddDoubleProperty("Distance Per Rotation",
                            [this] { return this->GetDistancePerRotation(); },
                            nullptr);
}
