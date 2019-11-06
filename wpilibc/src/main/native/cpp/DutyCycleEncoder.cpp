/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DutyCycleEncoder.h"

#include "frc/Counter.h"
#include "frc/DigitalSource.h"
#include "frc/DutyCycle.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

DutyCycleEncoder::DutyCycleEncoder(DutyCycle& dutyCycle)
    : m_dutyCycle{&dutyCycle, NullDeleter<DutyCycle>{}},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}
DutyCycleEncoder::DutyCycleEncoder(DutyCycle* dutyCycle)
    : m_dutyCycle{dutyCycle, NullDeleter<DutyCycle>{}},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}
DutyCycleEncoder::DutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle)
    : m_dutyCycle{std::move(dutyCycle)},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}

DutyCycleEncoder::DutyCycleEncoder(DigitalSource& digitalSource)
    : m_dutyCycle{std::make_shared<DutyCycle>(digitalSource)},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}
DutyCycleEncoder::DutyCycleEncoder(DigitalSource* digitalSource)
    : m_dutyCycle{std::make_shared<DutyCycle>(digitalSource)},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}
DutyCycleEncoder::DutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource)
    : m_dutyCycle{std::make_shared<DutyCycle>(digitalSource)},
      m_analogTrigger{m_dutyCycle.get()},
      m_counter{} {
  Init();
}

void DutyCycleEncoder::Init() {
  m_analogTrigger.SetLimitsDutyCycle(0.25, 0.75);
  m_counter.SetUpSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));

  SendableRegistry::GetInstance().AddLW(this, "DutyCycle Encoder",
                                        m_dutyCycle->GetSourceChannel());
}

int DutyCycleEncoder::GetRotations() const { return m_counter.Get(); }

double DutyCycleEncoder::Get() const {
  return GetRotations() + GetPositionInRotation() - m_positionOffset;
}

double DutyCycleEncoder::GetPositionInRotation() const {
  return m_dutyCycle->GetOutput();
}

double DutyCycleEncoder::GetPositionOffset() const { return m_positionOffset; }

void DutyCycleEncoder::SetDistancePerRotation(double distancePerRotation) {
  m_distancePerRotation = distancePerRotation;
}
double DutyCycleEncoder::GetDistancePerRotation() const {
  return m_distancePerRotation;
}
double DutyCycleEncoder::GetDistance() const {
  return Get() * GetDistancePerRotation();
}

int DutyCycleEncoder::GetFrequency() const {
  return m_dutyCycle->GetFrequency();
}

void DutyCycleEncoder::Reset() {
  m_counter.Reset();
  m_positionOffset = GetPositionInRotation();
}

bool DutyCycleEncoder::IsConnected() const {
  return GetFrequency() > m_frequencyThreshold;
}
void DutyCycleEncoder::SetConnectedFrequencyThreshold(int frequency) {
  if (frequency < 0) {
    frequency = 0;
  }
  m_frequencyThreshold = frequency;
}

void DutyCycleEncoder::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("AbsoluteEncoder");
  builder.AddDoubleProperty("Distance", [this] { return this->GetDistance(); },
                            nullptr);
  builder.AddDoubleProperty("Distance Per Rotation",
                            [this] { return this->GetDistancePerRotation(); },
                            nullptr);
}
