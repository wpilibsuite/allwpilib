// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/encoder/RotaryDutyCycleEncoder.h"

#include <wpi/NullDeleter.h>

#include "frc/DigitalInput.h"

using namespace frc;

RotaryDutyCycleEncoder::RotaryDutyCycleEncoder(
    int channel, decltype(1.0 / 1_tr) cyclesPerRevolution)
    : m_dutyCycle{std::make_shared<DutyCycle>(
          std::make_shared<DigitalInput>(channel))} {}

RotaryDutyCycleEncoder::RotaryDutyCycleEncoder(
    DutyCycle& dutyCycle, decltype(1.0 / 1_tr) cyclesPerRevolution)
    : m_dutyCycle{&dutyCycle, wpi::NullDeleter<DutyCycle>{}} {}

RotaryDutyCycleEncoder::RotaryDutyCycleEncoder(
    std::shared_ptr<DutyCycle> dutyCycle,
    decltype(1.0 / 1_tr) cyclesPerRevolution)
    : m_dutyCycle{std::move(dutyCycle)} {}

RotaryDutyCycleEncoder::RotaryDutyCycleEncoder(
    DigitalSource& digitalSource, decltype(1.0 / 1_tr) cyclesPerRevolution) {}

RotaryDutyCycleEncoder::RotaryDutyCycleEncoder(
    std::shared_ptr<DigitalSource> digitalSource,
    decltype(1.0 / 1_tr) cyclesPerRevolution) {}

units::radian_t RotaryDutyCycleEncoder::GetAngle() const {
  return 0_rad;
}

units::radians_per_second_t RotaryDutyCycleEncoder::GetAngularVelocity() const {
  return 0_rad_per_s;
}

void RotaryDutyCycleEncoder::SetInverted(bool inverted) {}

void RotaryDutyCycleEncoder::Reset() {}

void RotaryDutyCycleEncoder::SetSimDevice(HAL_SimDeviceHandle device) {}

int RotaryDutyCycleEncoder::GetFPGAIndex() const {
  return 0;
}
