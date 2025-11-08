// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "PortsInternal.h"
#include "wpi/hal/handles/DigitalHandleResource.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/nt/IntegerTopic.hpp"

namespace wpi::hal {

constexpr int32_t kPwmDisabled = 0;
constexpr int32_t kPwmAlwaysHigh = 0xFFFF;

enum class SmartIoMode {
  DigitalInput = 0,
  DigitalOutput = 1,
  AnalogInput = 2,
  PwmInput = 3,
  PwmOutput = 4,
  SingleCounterRising = 5,
  SingleCounterFalling = 6,
  AddressableLED = 13,
};

enum class PwmOutputPeriod {
  k20ms = 0,
  k10ms,
  k5ms,
  k2ms,
};

struct SmartIo {
  uint8_t channel;
  std::string previousAllocation;
  SmartIoMode currentMode{SmartIoMode::DigitalInput};
  wpi::nt::IntegerPublisher modePublisher;

  wpi::nt::IntegerPublisher setPublisher;
  wpi::nt::IntegerSubscriber getSubscriber;

  wpi::nt::IntegerPublisher periodSetPublisher;
  wpi::nt::IntegerSubscriber periodGetSubscriber;

  wpi::nt::IntegerPublisher ledcountPublisher;
  wpi::nt::IntegerPublisher ledoffsetPublisher;

  int32_t InitializeMode(SmartIoMode mode);
  int32_t SwitchDioDirection(bool input);

  int32_t SetDigitalOutput(bool value);
  int32_t GetDigitalInput(bool* value);

  int32_t GetPwmInputMicroseconds(uint16_t* microseconds);
  int32_t GetPwmInputPeriodMicroseconds(uint16_t* microseconds);

  int32_t SetPwmOutputPeriod(PwmOutputPeriod period);

  int32_t SetPwmMicroseconds(uint16_t microseconds);
  int32_t GetPwmMicroseconds(uint16_t* microseconds);

  int32_t GetAnalogInput(uint16_t* value);

  int32_t GetCounter(int32_t* count);

  int32_t SetLedStart(int32_t start);
  int32_t SetLedLength(int32_t length);
};

extern DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>*
    smartIoHandles;

extern wpi::util::mutex smartIoMutex;

}  // namespace wpi::hal
