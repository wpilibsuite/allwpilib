// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/encoder/LinearDutyCycleEncoder.h"

#include <wpi/NullDeleter.h>

#include "frc/DigitalInput.h"

using namespace frc;

LinearDutyCycleEncoder::LinearDutyCycleEncoder(int channel,
                                               decltype(1.0 /
                                                        1_m) cyclesPerMeter)
    : m_dutyCycle{std::make_shared<DutyCycle>(
          std::make_shared<DigitalInput>(channel))} {
  Init();
}

LinearDutyCycleEncoder::LinearDutyCycleEncoder(DutyCycle& dutyCycle,
                                               decltype(1.0 /
                                                        1_m) cyclesPerMeter)
    : m_dutyCycle{&dutyCycle, wpi::NullDeleter<DutyCycle>{}} {
  Init();
}

LinearDutyCycleEncoder::LinearDutyCycleEncoder(
    std::shared_ptr<DutyCycle> dutyCycle, decltype(1.0 / 1_m) cyclesPerMeter)
    : m_dutyCycle{std::move(dutyCycle)} {
  Init();
}

LinearDutyCycleEncoder::LinearDutyCycleEncoder(DigitalSource& digitalSource,
                                               decltype(1.0 /
                                                        1_m) cyclesPerMeter) {
  Init();
}

LinearDutyCycleEncoder::LinearDutyCycleEncoder(
    std::shared_ptr<DigitalSource> digitalSource,
    decltype(1.0 / 1_m) cyclesPerMeter) {}

units::meter_t LinearDutyCycleEncoder::GetDisplacement() const {
  return 0_m;
}

units::meters_per_second_t LinearDutyCycleEncoder::GetVelocity() const {
  return 0_mps;
}

void LinearDutyCycleEncoder::SetInverted(bool inverted) {}

void LinearDutyCycleEncoder::Reset() {}

void LinearDutyCycleEncoder::SetSimDevice(HAL_SimDeviceHandle device) {}

int LinearDutyCycleEncoder::GetFPGAIndex() const {
  return 0;
}

void LinearDutyCycleEncoder::Init() {}
