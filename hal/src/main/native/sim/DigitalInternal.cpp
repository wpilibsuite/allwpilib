// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DigitalInternal.hpp"

#include "PortsInternal.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/DigitalHandleResource.hpp"
#include "wpi/hal/handles/HandlesInternal.hpp"

namespace wpi::hal {

DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                      NUM_DIGITAL_CHANNELS + NUM_PWM_HEADERS>*
    digitalChannelHandles;

namespace init {
void InitializeDigitalInternal() {
  static DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                               NUM_DIGITAL_CHANNELS + NUM_PWM_HEADERS>
      dcH;
  digitalChannelHandles = &dcH;
}
}  // namespace init

int32_t GetDigitalInputChannel(HAL_DigitalHandle handle, int32_t* status) {
  auto digital = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (digital == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  return digital->channel;
}
}  // namespace wpi::hal
