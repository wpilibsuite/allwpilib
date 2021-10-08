// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/encoder/RotaryAnalogEncoder.h"

#include <wpi/NullDeleter.h>

using namespace frc;

RotaryAnalogEncoder::RotaryAnalogEncoder(int channel,
                                         decltype(1_V /
                                                  1_tr) voltsPerRevolution)
    : RotaryAnalogEncoder{std::make_shared<AnalogInput>(channel),
                          voltsPerRevolution} {}

RotaryAnalogEncoder::RotaryAnalogEncoder(AnalogInput& analogInput,
                                         decltype(1_V /
                                                  1_tr) voltsPerRevolution)
    : m_analogInput{&analogInput, wpi::NullDeleter<AnalogInput>{}},
      m_analogTrigger{m_analogInput.get()} {}

RotaryAnalogEncoder::RotaryAnalogEncoder(
    std::shared_ptr<AnalogInput> analogInput,
    decltype(1_V / 1_tr) voltsPerRevolution)
    : m_analogInput{std::move(analogInput)},
      m_analogTrigger{m_analogInput.get()} {}

units::radian_t RotaryAnalogEncoder::GetAngle() const {
  return 0_rad;
}

units::radians_per_second_t RotaryAnalogEncoder::GetAngularVelocity() const {
  return 0_rad_per_s;
}

void RotaryAnalogEncoder::SetInverted(bool inverted) {}

void RotaryAnalogEncoder::Reset() {}

void RotaryAnalogEncoder::SetSimDevice(HAL_SimDeviceHandle device) {}

int RotaryAnalogEncoder::GetChannel() const {
  return m_analogInput->GetChannel();
}
