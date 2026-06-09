// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SmartIo.hpp"

#include "HALInitializer.hpp"
#include "mrclib/SmartIO.h"
#include "wpi/hal/AddressableLEDTypes.h"
#include "wpi/hal/Errors.h"

namespace wpi::hal {

wpi::util::mutex smartIoMutex;
DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>* smartIoHandles;

namespace init {
void InitializeSmartIo() {
  static DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo> dcH;
  smartIoHandles = &dcH;
}

}  // namespace init

namespace {
struct SmartIoInitializer {
  MRC_Status status = MRC_SmartIO_Initialize();
};
}  // namespace

SmartIo::~SmartIo() noexcept {
  MRC_SmartIO_Close(channel);
}

int32_t SmartIo::InitializeMode(MRC_SmartIOMode mode) {
  static SmartIoInitializer mrcSmartIo;

  if (mrcSmartIo.status != 0) {
    return mrcSmartIo.status;
  }

  MRC_Status ret = MRC_SmartIO_InitializeMode(channel, mode);
  if (ret == 0) {
    currentMode = mode;
  }
  return ret;
}

int32_t SmartIo::SwitchDioDirection(bool input) {
  MRC_Status ret = MRC_SmartIO_SwitchDirection(channel, input);
  if (ret == 0) {
    currentMode = input ? MRC_SmartIOMode::MRC_SmartIOMode_DigitalInput
                        : MRC_SmartIOMode::MRC_SmartIOMode_DigitalOutput;
  }
  return ret;
}

int32_t SmartIo::SetDigitalOutput(bool value) {
  return MRC_SmartIO_SetDigitalOutput(channel, value);
}

int32_t SmartIo::GetDigitalInput(bool* value) {
  MRC_Bool val;
  int32_t status = MRC_SmartIO_GetDigitalInput(channel, &val);
  if (status == 0) {
    *value = val ? true : false;
  }
  return status;
}

int32_t SmartIo::GetPwmInputMicroseconds(uint16_t* microseconds) {
  int32_t microsecondsInt;
  int32_t status =
      MRC_SmartIO_GetPwmInputMicroseconds(channel, &microsecondsInt);
  if (status == 0) {
    *microseconds = microsecondsInt;
  }
  return status;
}

int32_t SmartIo::GetPwmInputPeriodMicroseconds(uint16_t* microseconds) {
  int32_t microsecondsInt;
  int32_t status =
      MRC_SmartIO_GetPwmInputPeriodMicroseconds(channel, &microsecondsInt);
  if (status == 0) {
    *microseconds = microsecondsInt;
  }
  return status;
}

int32_t SmartIo::SetPwmOutputPeriod(MRC_PwmOutputPeriod period) {
  return MRC_SmartIO_SetPwmOutputPeriod(channel, period);
}

int32_t SmartIo::SetPwmOutputMicroseconds(uint16_t microseconds) {
  MRC_Status ret = MRC_SmartIO_SetPwmOutputMicroseconds(channel, microseconds);
  if (ret != 0) {
    setPwmOutputMicrosecondsValue = 0;
  } else {
    setPwmOutputMicrosecondsValue = microseconds;
  }
  return ret;
}

int32_t SmartIo::GetPwmOutputMicroseconds(uint16_t* microseconds) {
  *microseconds = setPwmOutputMicrosecondsValue;
  return MRC_STATUS_SUCCESS;
}

int32_t SmartIo::GetAnalogInput(uint16_t* value) {
  int32_t valueInt;
  int32_t status = MRC_SmartIO_GetAnalogInput(channel, &valueInt);
  if (status == 0) {
    *value = valueInt;
  }
  return status;
}

int32_t SmartIo::GetCounter(int32_t* value) {
  int32_t valueInt;
  int32_t status = MRC_SmartIO_GetCounter(channel, &valueInt);
  if (status == 0) {
    *value = valueInt;
  }
  return status;
}

int32_t SmartIo::SetLedStart(int32_t start) {
  return MRC_SmartIO_SetLedStartIndex(channel, start);
}

int32_t SmartIo::SetLedLength(int32_t length) {
  return MRC_SmartIO_SetLedLength(channel, length);
}

}  // namespace wpi::hal
