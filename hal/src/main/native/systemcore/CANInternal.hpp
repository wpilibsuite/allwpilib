// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/CANAPITypes.h"

namespace wpi::hal {

/**
 * Callback invoked before an internal callback-aware CAN message is sent.
 *
 * The callback may modify the message for the current send. Returning false
 * suppresses the current send without stopping future periodic callbacks. CAN
 * read APIs may be called from the callback, but APIs that send, stop, or
 * schedule CAN messages must not be called.
 */
using CANPeriodicSendCallback = HAL_Bool (*)(void* param,
                                             HAL_CANMessage* message);

bool InitializeCanBuses();

void SendCANMessageWithPeriodicCallback(int32_t busId, uint32_t messageId,
                                        const HAL_CANMessage* message,
                                        int32_t periodMs,
                                        CANPeriodicSendCallback callback,
                                        void* param, int32_t* status);

void WriteCANPacketRepeatingWithCallback(HAL_CANHandle handle, int32_t apiId,
                                         const HAL_CANMessage* message,
                                         int32_t repeatMs,
                                         CANPeriodicSendCallback callback,
                                         void* param, int32_t* status);

}  // namespace wpi::hal
