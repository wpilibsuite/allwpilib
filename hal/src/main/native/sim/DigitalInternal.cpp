// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DigitalInternal.h"

#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/DigitalHandleResource.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {

DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                      kNumDigitalChannels + kNumPWMHeaders>*
    digitalChannelHandles;

namespace init {
void InitializeDigitalInternal() {
  static DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                               kNumDigitalChannels + kNumPWMHeaders>
      dcH;
  digitalChannelHandles = &dcH;
}
}  // namespace init

int32_t remapMXPChannel(int32_t channel) {
  return channel - 10;
}

int32_t remapMXPPWMChannel(int32_t channel) {
  if (channel < 14) {
    return channel - 10;  // first block of 4 pwms (MXP 0-3)
  } else {
    return channel - 6;  // block of PWMs after SPI
  }
}

int32_t GetDigitalInputChannel(HAL_DigitalHandle handle, int32_t* status) {
  auto digital = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (digital == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  return digital->channel;
}
}  // namespace hal
