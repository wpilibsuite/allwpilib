/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DutyCycleEncoder.h"
#include "frc/DutyCycle.h"
#include "frc/DigitalSource.h"
#include "frc/Counter.h"
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
  m_analogTrigger.SetLimitsDutyCycle(0.05, 0.95);
  m_counter.SetUpSource(m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));
}


int DutyCycleEncoder::GetRotations() const {
  return m_counter.Get();
}

int DutyCycleEncoder::GetFrequency() const {
  return m_dutyCycle->GetFrequency();
}

void DutyCycleEncoder::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Encoder");
  builder.AddDoubleProperty("Frequency",
                            [this] { return this->GetFrequency(); }, nullptr);
  builder.AddDoubleProperty("Rotations", [this] { return this->GetRotations(); },
                            nullptr);

}
