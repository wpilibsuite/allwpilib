// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CAN2027.h"

#include "hal/CAN.h"
#include "hal/Errors.h"
#include "mockdata/CanDataInternal.h"

using namespace hal;

extern "C" {

void HAL_CAN2027_SendMessage(int32_t busId, uint32_t messageId,
                             const struct HAL_CAN2027Message* message,
                             int32_t periodMs, int32_t* status) {
  if (busId != 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  if (message->flags != 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  SimCanData->sendMessage(messageId, message->data, message->dataSize, periodMs,
                          status);
}
void HAL_CAN2027_ReceiveMessage(int32_t busId, uint32_t messageId,
                                struct HAL_CAN2027ReceiveMessage* message,
                                int32_t* status) {
  if (busId != 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  // Use a data size of 42 as call check. Difficult to add check to invoke
  // handler
  message->message.dataSize = 42;
  auto tmpStatus = *status;
  uint32_t timeStamp = 0;
  SimCanData->receiveMessage(&messageId, 0x1FFFFFFF, message->message.data,
                             &message->message.dataSize, &timeStamp, status);
  // If no handler invoked, return message not found
  if (message->message.dataSize == 42 && *status == tmpStatus) {
    message->message.dataSize = 0;
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
  }
  message->timeStamp = timeStamp * 1000;
}
HAL_CAN2027StreamHandle HAL_CAN2027_OpenStreamSession(int32_t busId,
                                                      uint32_t messageId,
                                                      uint32_t messageIDMask,
                                                      uint32_t maxMessages,
                                                      int32_t* status) {
  if (busId != 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  uint32_t sessionHandle = 0;
  SimCanData->openStreamSession(&sessionHandle, messageId, messageIDMask,
                                maxMessages, status);
  return sessionHandle;
}
void HAL_CAN2027_CloseStreamSession(HAL_CAN2027StreamHandle sessionHandle) {
  SimCanData->closeStreamSession(sessionHandle);
}
void HAL_CAN2027_ReadStreamSession(HAL_CAN2027StreamHandle sessionHandle,
                                   struct HAL_CAN2027StreamMessage* messages,
                                   uint32_t messagesToRead,
                                   uint32_t* messagesRead, int32_t* status) {
  std::unique_ptr<HAL_CANStreamMessage[]> ncMessages =
      std::make_unique<HAL_CANStreamMessage[]>(messagesToRead);
  *messagesRead = 0;
  SimCanData->readStreamSession(sessionHandle, ncMessages.get(), messagesToRead,
                                messagesRead, status);
  for (uint32_t i = 0; i < *messagesRead; i++) {
    messages[i].messageId = ncMessages[i].messageID;
    messages[i].message.timeStamp = ncMessages[i].timeStamp * 1000;
    messages[i].message.message.dataSize = ncMessages[i].dataSize;
    std::copy_n(ncMessages[i].data, ncMessages[i].dataSize,
                messages[i].message.message.data);
  }
}
void HAL_CAN_GetCAN2027Status(int32_t busId, float* percentBusUtilization,
                              uint32_t* busOffCount, uint32_t* txFullCount,
                              uint32_t* receiveErrorCount,
                              uint32_t* transmitErrorCount, int32_t* status) {
  if (busId != 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  SimCanData->getCANStatus(percentBusUtilization, busOffCount, txFullCount,
                           receiveErrorCount, transmitErrorCount, status);
}

}  // extern "C"
