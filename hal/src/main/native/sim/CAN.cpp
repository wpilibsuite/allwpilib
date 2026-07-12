// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/CAN.h"

#include "mockdata/CanDataInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeCAN() {}
}  // namespace wpi::hal::init

extern "C" {

void HAL_CAN_SendMessage(int32_t busId, uint32_t messageId,
                         const struct HAL_CANMessage* message, int32_t periodMs,
                         int32_t* status) {
  SimCanData->sendMessage(busId, messageId, message, periodMs, status);
}
void HAL_CAN_ReceiveMessage(int32_t busId, uint32_t messageId,
                            struct HAL_CANReceiveMessage* message,
                            int32_t* status) {
  // Use a data size of 100 as call check. Difficult to add check to invoke
  // handler
  message->message.dataSize = 100;
  auto tmpStatus = *status;
  SimCanData->receiveMessage(busId, messageId, message, status);
  // If no handler invoked, return message not found
  if (message->message.dataSize == 100 && *status == tmpStatus) {
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
  }
}
HAL_CANStreamHandle HAL_CAN_OpenStreamSession(int32_t busId, uint32_t messageId,
                                              uint32_t messageIDMask,
                                              uint32_t maxMessages,
                                              int32_t* status) {
  HAL_CANStreamHandle handle = 0;
  SimCanData->openStreamSession(&handle, busId, messageId, messageIDMask,
                                maxMessages, status);
  return handle;
}
void HAL_CAN_CloseStreamSession(HAL_CANStreamHandle sessionHandle) {
  SimCanData->closeStreamSession(sessionHandle);
}
void HAL_CAN_ReadStreamSession(HAL_CANStreamHandle sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  SimCanData->readStreamSession(sessionHandle, messages, messagesToRead,
                                messagesRead, status);
}
void HAL_CAN_GetCANStatus(int32_t busId, float* percentBusUtilization,
                          uint32_t* busOffCount, uint32_t* txFullCount,
                          uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  SimCanData->getCANStatus(busId, percentBusUtilization, busOffCount,
                           txFullCount, receiveErrorCount, transmitErrorCount,
                           status);
}

}  // extern "C"
