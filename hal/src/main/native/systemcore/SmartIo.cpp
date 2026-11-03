// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SmartIo.hpp"

#include "HALInitializer.hpp"
#include "mrclib/SmartIO.h"
#include "wpi/hal/AddressableLEDTypes.h"

namespace wpi::hal {

wpi::util::mutex smartIoMutex;
DigitalHandleResource<HAL_DigitalHandle, SmartIo, NUM_SMART_IO>* smartIoHandles;

namespace init {
void InitializeSmartIo() {
  static DigitalHandleResource<HAL_DigitalHandle, SmartIo, NUM_SMART_IO> dcH;
  smartIoHandles = &dcH;
}

}  // namespace init

SmartIo::~SmartIo() noexcept {
  if (closeOnDestroy) {
    MRC_SmartIO_Close(channel);
  }
}

int32_t SmartIo::InitializeMode(MRC_SmartIOMode mode) {
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

int32_t SmartIo::SwitchCounterEdge(bool risingEdge) {
  MRC_Status ret = MRC_SmartIO_SwitchCounterEdge(channel, risingEdge);
  if (ret == 0) {
    currentMode = risingEdge
                      ? MRC_SmartIOMode::MRC_SmartIOMode_SingleCounterRising
                      : MRC_SmartIOMode::MRC_SmartIOMode_SingleCounterFalling;
  }
  return ret;
}

int32_t SmartIo::SetRateWindow(int32_t windowMilliseconds) {
  return MRC_SmartIO_SetRateWindow(channel, windowMilliseconds);
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

int32_t SmartIo::ResetCounter() {
  int32_t count;
  int32_t status = MRC_SmartIO_GetCounter(channel, &count);
  if (status == 0) {
    counterResetCount = count;
  }
  return status;
}

int32_t SmartIo::GetCounter(int32_t* value) {
  int32_t count;
  int32_t status = MRC_SmartIO_GetCounter(channel, &count);
  if (status == 0) {
    *value =
        static_cast<int32_t>(static_cast<int64_t>(count) - counterResetCount);
  }
  return status;
}

int32_t SmartIo::GetCounterRate(int32_t* value) {
  int32_t rate;
  int32_t status = MRC_SmartIO_GetCounterRate(channel, &rate);
  if (status == 0) {
    *value = rate;
  }
  return status;
}

int32_t SmartIo::GetQuadrature(int32_t* value) {
  int32_t valueInt;
  int32_t status = MRC_SmartIO_GetQuadrature(channel, &valueInt);
  if (status == 0) {
    *value = valueInt;
  }
  return status;
}

int32_t SmartIo::GetQuadratureRate(int32_t* value) {
  int32_t valueInt;
  int32_t status = MRC_SmartIO_GetQuadratureRate(channel, &valueInt);
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
