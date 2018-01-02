/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInternal.h"

#include "ConstantsInternal.h"
#include "HAL/AnalogTrigger.h"
#include "HAL/HAL.h"
#include "HAL/Ports.h"
#include "PortsInternal.h"

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

/**
 * Map DIO channel numbers from their physical number (10 to 26) to their
 * position in the bit field.
 */
int32_t remapMXPChannel(int32_t channel) { return channel - 10; }

int32_t remapMXPPWMChannel(int32_t channel) {
  if (channel < 14) {
    return channel - 10;  // first block of 4 pwms (MXP 0-3)
  } else {
    return channel - 6;  // block of PWMs after SPI
  }
}

/**
 * remap the digital source channel and set the module.
 * If it's an analog trigger, determine the module from the high order routing
 * channel else do normal digital input remapping based on channel number
 * (MXP)
 */
bool remapDigitalSource(HAL_Handle digitalSourceHandle,
                        HAL_AnalogTriggerType analogTriggerType,
                        uint8_t& channel, uint8_t& module,
                        bool& analogTrigger) {
  if (isHandleType(digitalSourceHandle, HAL_HandleEnum::AnalogTrigger)) {
    // If handle passed, index is not negative
    int32_t index = getHandleIndex(digitalSourceHandle);
    channel = (index << 2) + analogTriggerType;
    module = channel >> 4;
    analogTrigger = true;
    return true;
  } else if (isHandleType(digitalSourceHandle, HAL_HandleEnum::DIO)) {
    int32_t index = getHandleIndex(digitalSourceHandle);
    if (index >= kNumDigitalHeaders) {
      channel = remapMXPChannel(index);
      module = 1;
    } else {
      channel = index;
      module = 0;
    }
    analogTrigger = false;
    return true;
  } else {
    return false;
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
