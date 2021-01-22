// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CAN.h"

#include "mockdata/CanDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeCAN() {}
}  // namespace hal::init

extern "C" {

void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status) {
  SimCanData->sendMessage(messageID, data, dataSize, periodMs, status);
}
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status) {
  // Use a data size of 42 as call check. Difficult to add check to invoke
  // handler
  *dataSize = 42;
  auto tmpStatus = *status;
  SimCanData->receiveMessage(messageID, messageIDMask, data, dataSize,
                             timeStamp, status);
  // If no handler invoked, return message not found
  if (*dataSize == 42 && *status == tmpStatus) {
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
  }
}
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status) {
  SimCanData->openStreamSession(sessionHandle, messageID, messageIDMask,
                                maxMessages, status);
}
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle) {
  SimCanData->closeStreamSession(sessionHandle);
}
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  SimCanData->readStreamSession(sessionHandle, messages, messagesToRead,
                                messagesRead, status);
}
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  SimCanData->getCANStatus(percentBusUtilization, busOffCount, txFullCount,
                           receiveErrorCount, transmitErrorCount, status);
}

}  // extern "C"
