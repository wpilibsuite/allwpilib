// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <networktables/IntegerTopic.h>

#include "PortsInternal.h"
#include "hal/handles/DigitalHandleResource.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

constexpr int32_t kPwmDisabled = 0;
constexpr int32_t kPwmAlwaysHigh = 0xFFFF;

enum class SmartIoMode {
  DigitalInput = 0,
  DigitalOutput,
  AnalogInput,
  PwmInput,
  PwmOutput,
  SingleCounterRising,
  SingleCounterFalling,
};

enum class PwmOutputPeriod {
  k20ms = 0,
  k10ms,
  k5ms,
  k2ms,
};

struct SmartIo {
  uint8_t channel;
  bool configSet = false;
  bool eliminateDeadband = false;
  int32_t maxPwm = 0;
  int32_t deadbandMaxPwm = 0;
  int32_t centerPwm = 0;
  int32_t deadbandMinPwm = 0;
  int32_t minPwm = 0;
  std::string previousAllocation;
  SmartIoMode currentMode{SmartIoMode::DigitalInput};
  nt::IntegerPublisher modePublisher;

  nt::IntegerPublisher setPublisher;
  nt::IntegerSubscriber getSubscriber;

  nt::IntegerPublisher periodPublisher;
  nt::IntegerSubscriber frequencySubscriber;

  int32_t InitializeMode(SmartIoMode mode);
  int32_t SwitchDioDirection(bool input);

  int32_t SetDigitalOutput(bool value);
  int32_t GetDigitalInput(bool* value);

  int32_t GetPwmInputMicroseconds(uint16_t* microseconds);

  int32_t SetPwmOutputPeriod(PwmOutputPeriod period);

  int32_t SetPwmMicroseconds(uint16_t microseconds);
  int32_t GetPwmMicroseconds(uint16_t* microseconds);

  int32_t GetAnalogInput(uint16_t* value);

  int32_t GetCounter(int32_t* count);
};

extern DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>*
    smartIoHandles;

extern wpi::mutex smartIoMutex;

}  // namespace hal
