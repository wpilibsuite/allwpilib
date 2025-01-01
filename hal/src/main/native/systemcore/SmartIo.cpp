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

<<<<<<< HEAD
  modePublisher = inst.GetDoubleTopic("/io/type" + channelString).Publish();
  getSubscriber = inst.GetDoubleTopic("/io/valread" + channelString)
                      .Subscribe(0.0, options);
  setPublisher =
      inst.GetDoubleTopic("/io/valset" + channelString).Publish(options);
  setPublisher.Set(0);
=======
  modePublisher = inst.GetIntegerTopic(subTableString + "type").Publish();
  getSubscriber =
      inst.GetIntegerTopic(subTableString + "valget").Subscribe(0.0, options);
>>>>>>> 2027

  currentMode = mode;
  switch (mode) {
    case SmartIoMode::PWMOutput:
<<<<<<< HEAD
      modePublisher.Set(3);
      pwmMinPublisher =
          inst.GetDoubleTopic("/io/pwmmin" + channelString).Publish();
      pwmMinPublisher.Set(0);
      pwmMaxPublisher =
          inst.GetDoubleTopic("/io/pwmmax" + channelString).Publish();
      pwmMaxPublisher.Set(4096);
=======
      modePublisher.Set(4);
      setPublisher =
          inst.GetIntegerTopic(subTableString + "valset").Publish(options);
      setPublisher.Set(0);
>>>>>>> 2027
      return 0;

    case SmartIoMode::DigitalInput:
      modePublisher.Set(0);
      return 0;

    case SmartIoMode::DigitalOutput:
      modePublisher.Set(1);
      return 0;

    case SmartIoMode::Disabled:
      modePublisher.Set(0);
      return 0;

    default:

      return INCOMPATIBLE_STATE;
  }
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
