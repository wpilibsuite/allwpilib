// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/encoder/LinearAnalogEncoder.h"

#include <wpi/NullDeleter.h>

#include "frc/Errors.h"

using namespace frc;

LinearAnalogEncoder::LinearAnalogEncoder(int channel,
                                         decltype(1_V / 1_m) voltsPerMeter)
    : LinearAnalogEncoder{std::make_shared<AnalogInput>(channel),
                          voltsPerMeter} {}

LinearAnalogEncoder::LinearAnalogEncoder(AnalogInput& analogInput,
                                         decltype(1_V / 1_m) voltsPerMeter)
    : m_analogInput{&analogInput, wpi::NullDeleter<AnalogInput>{}},
      m_analogTrigger{m_analogInput.get()} {
  Init(voltsPerMeter);
}

LinearAnalogEncoder::LinearAnalogEncoder(
    std::shared_ptr<AnalogInput> analogInput, decltype(1_V / 1_m) voltsPerMeter)
    : m_analogInput{std::move(analogInput)},
      m_analogTrigger{m_analogInput.get()} {
  Init(voltsPerMeter);
}

units::meter_t LinearAnalogEncoder::GetDisplacement() const {
  return 0_m;
}

units::meters_per_second_t LinearAnalogEncoder::GetVelocity() const {
  return 0_mps;
}

void LinearAnalogEncoder::SetInverted(bool inverted) {}

void LinearAnalogEncoder::Reset() {}

void LinearAnalogEncoder::SetSimDevice(HAL_SimDeviceHandle device) {}

int LinearAnalogEncoder::GetChannel() const {
  return m_analogInput->GetChannel();
}

void LinearAnalogEncoder::Init(decltype(1_V / 1_m) voltsPerMeter) {
  m_simDevice =
      hal::SimDevice{"LinearAnalogEncoder", m_analogInput->GetChannel()};

  if (m_simDevice) {
    m_simPosition = m_simDevice.CreateDouble("Position", false, 0.0);
  }

  m_analogTrigger.SetLimitsVoltage(1.25, 3.75);
  m_counter.SetUpSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kRisingPulse));
  m_counter.SetDownSource(
      m_analogTrigger.CreateOutput(AnalogTriggerType::kFallingPulse));

  wpi::SendableRegistry::AddLW(this, "Linear Analog Encoder",
                               m_analogInput->GetChannel());
}
