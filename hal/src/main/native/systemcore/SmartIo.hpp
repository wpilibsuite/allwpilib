// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "PortsInternal.hpp"
#include "mrclib/SmartIO.h"
#include "wpi/hal/handles/DigitalHandleResource.hpp"

namespace wpi::hal {

constexpr int32_t kPwmDisabled = 0;
constexpr int32_t kPwmAlwaysHigh = 0xFFFF;

struct SmartIo {
  ~SmartIo() noexcept;
  uint8_t channel;
  bool closeOnDestroy{true};
  std::string previousAllocation;
  MRC_SmartIOMode currentMode{MRC_SmartIOMode::MRC_SmartIOMode_DigitalInput};
  uint16_t setPwmOutputMicrosecondsValue{0};

  int32_t InitializeMode(MRC_SmartIOMode mode);
  int32_t SwitchDioDirection(bool input);

  int32_t SetDigitalOutput(bool value);
  int32_t GetDigitalInput(bool* value);

  int32_t GetPwmInputMicroseconds(uint16_t* microseconds);
  int32_t GetPwmInputPeriodMicroseconds(uint16_t* microseconds);

  int32_t SetPwmOutputPeriod(MRC_PwmOutputPeriod period);
  int32_t SetPwmOutputMicroseconds(uint16_t microseconds);
  int32_t GetPwmOutputMicroseconds(uint16_t* microseconds);

  int32_t GetAnalogInput(uint16_t* value);

  int32_t GetCounter(int32_t* count);
  int32_t GetQuadrature(int32_t* count);
  int32_t GetQuadratureRate(int32_t* rate);

  int32_t SetLedStart(int32_t start);
  int32_t SetLedLength(int32_t length);
};

extern DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>*
    smartIoHandles;

extern wpi::util::mutex smartIoMutex;

}  // namespace wpi::hal
