// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SmartIo.h"

#include <atomic>

#include "HALInitializer.h"
#include "SystemServer.h"

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
      inst.GetIntegerTopic(subTableString + "valget").Subscribe(0.0, options);

  currentMode = mode;
  switch (mode) {
    case SmartIoMode::PWMOutput:
      modePublisher.Set(4);
      setPublisher =
          inst.GetIntegerTopic(subTableString + "valset").Publish(options);
      setPublisher.Set(0);
      return 0;

    default:

      return INCOMPATIBLE_STATE;
  }
}

int32_t SmartIo::SetPwmMicroseconds(uint16_t microseconds) {
  if (currentMode != SmartIoMode::PWMOutput) {
    return INCOMPATIBLE_STATE;
  }

  // TODO(thad) add support for always on signal

  if (microseconds > 4095) {
    microseconds = 4095;
  }

  setPublisher.Set(microseconds);

  return 0;
}

int32_t SmartIo::GetPwmMicroseconds(uint16_t* microseconds) {
  if (currentMode != SmartIoMode::PWMOutput) {
    return INCOMPATIBLE_STATE;
  }

  int val = getSubscriber.Get();

  // Get to 0-2, then scale to 0-4096;
  *microseconds = val;

  return 0;
}

}  // namespace hal
