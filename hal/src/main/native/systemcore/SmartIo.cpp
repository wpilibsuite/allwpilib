// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SmartIo.h"

#include <atomic>

#include "HALInitializer.h"
#include "SystemServerInternal.h"

namespace hal {

wpi::mutex smartIoMutex;
DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>* smartIoHandles;

namespace init {
void InitializeSmartIo() {
  static DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo> dcH;
  smartIoHandles = &dcH;
}
}  // namespace init

int32_t SmartIo::InitializeMode(SmartIoMode mode) {
  auto inst = hal::GetSystemServer();

  nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;

  auto channelString = std::to_string(channel);
  auto subTableString = "/io/" + channelString + "/";

  modePublisher = inst.GetIntegerTopic(subTableString + "type").Publish();
  getSubscriber =
      inst.GetIntegerTopic(subTableString + "valget").Subscribe(0, options);
  frequencySubscriber =
      inst.GetIntegerTopic(subTableString + "freqget").Subscribe(0, options);
  setPublisher =
      inst.GetIntegerTopic(subTableString + "valset").Publish(options);
  periodPublisher =
      inst.GetIntegerTopic(subTableString + "periodset").Publish(options);

  currentMode = mode;
  switch (mode) {
    // These need to set a 0 output
    case SmartIoMode::DigitalOutput:
    case SmartIoMode::PwmOutput:
      setPublisher.Set(0);
      break;

    // These don't need to set any value
    case SmartIoMode::DigitalInput:
    case SmartIoMode::AnalogInput:
    case SmartIoMode::PwmInput:
    case SmartIoMode::SingleCounterRising:
    case SmartIoMode::SingleCounterFalling:
      break;

    default:

      return INCOMPATIBLE_STATE;
  }

  modePublisher.Set(static_cast<int>(mode));
  return 0;
}

int32_t SmartIo::SwitchDioDirection(bool input) {
  if (currentMode != SmartIoMode::DigitalInput &&
      currentMode != SmartIoMode::DigitalOutput) {
    return INCOMPATIBLE_STATE;
  }

  modePublisher.Set(input ? 0 : 1);
  currentMode = input ? SmartIoMode::DigitalInput : SmartIoMode::DigitalOutput;
  return 0;
}

int32_t SmartIo::SetDigitalOutput(bool value) {
  if (currentMode != SmartIoMode::DigitalInput &&
      currentMode != SmartIoMode::DigitalOutput) {
    return INCOMPATIBLE_STATE;
  }
  setPublisher.Set(value ? 255.0 : 0.0);
  return 0;
}

int32_t SmartIo::GetDigitalInput(bool* value) {
  if (currentMode != SmartIoMode::DigitalInput &&
      currentMode != SmartIoMode::DigitalOutput) {
    return INCOMPATIBLE_STATE;
  }
  *value = getSubscriber.Get() != 0;
  return 0;
}

int32_t SmartIo::GetPwmInputMicroseconds(uint16_t* microseconds) {
  if (currentMode != SmartIoMode::PwmInput) {
    return INCOMPATIBLE_STATE;
  }

  int val = getSubscriber.Get();
  *microseconds = val;

  return 0;
}

int32_t SmartIo::SetPwmOutputPeriod(PwmOutputPeriod period) {
  if (currentMode != SmartIoMode::PwmOutput) {
    return INCOMPATIBLE_STATE;
  }

  switch (period) {
    case PwmOutputPeriod::k20ms:
    case PwmOutputPeriod::k10ms:
    case PwmOutputPeriod::k5ms:
    case PwmOutputPeriod::k2ms:
      periodPublisher.Set(static_cast<int>(period));
      return 0;

    default:
      return PARAMETER_OUT_OF_RANGE;
  }
}

int32_t SmartIo::SetPwmMicroseconds(uint16_t microseconds) {
  if (currentMode != SmartIoMode::PwmOutput) {
    return INCOMPATIBLE_STATE;
  }

  if (microseconds > 4095) {
    microseconds = 4095;
  }

  setPublisher.Set(microseconds);

  return 0;
}

int32_t SmartIo::GetPwmMicroseconds(uint16_t* microseconds) {
  if (currentMode != SmartIoMode::PwmOutput) {
    return INCOMPATIBLE_STATE;
  }

  int val = getSubscriber.Get();

  // Get to 0-2, then scale to 0-4096;
  *microseconds = val;

  return 0;
}

int32_t SmartIo::GetAnalogInput(uint16_t* value) {
  if (currentMode != SmartIoMode::AnalogInput) {
    return INCOMPATIBLE_STATE;
  }

  int val = getSubscriber.Get();

  *value = val;

  return 0;
}

}  // namespace hal
