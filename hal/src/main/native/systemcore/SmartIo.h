// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "PortsInternal.h"
#include "hal/handles/DigitalHandleResource.h"
#include "hal/handles/HandlesInternal.h"
#include "networktables/IntegerTopic.h"

namespace hal {

constexpr int32_t kPwmDisabled = 0;
constexpr int32_t kPwmAlwaysHigh = 0xFFFF;

enum class SmartIoMode {
  DigitalInput,
  PWMOutput,
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

  int32_t InitializeMode(SmartIoMode mode);
  int32_t SetPwmMicroseconds(uint16_t microseconds);
  int32_t GetPwmMicroseconds(uint16_t* microseconds);
};

extern DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>*
    smartIoHandles;

extern wpi::mutex smartIoMutex;

}  // namespace hal
