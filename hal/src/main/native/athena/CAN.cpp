// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CAN.h"

#include <FRC_NetworkCommunication/CANSessionMux.h>

namespace hal::init {
void InitializeCAN() {}
}  // namespace hal::init

extern "C" {

void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status) {
  FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, data, dataSize,
                                                     periodMs, status);
}
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status) {
  FRC_NetworkCommunication_CANSessionMux_receiveMessage(
      messageID, messageIDMask, data, dataSize, timeStamp, status);
}
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status) {
  FRC_NetworkCommunication_CANSessionMux_openStreamSession(
      sessionHandle, messageID, messageIDMask, maxMessages, status);
}
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle) {
  FRC_NetworkCommunication_CANSessionMux_closeStreamSession(sessionHandle);
}
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  FRC_NetworkCommunication_CANSessionMux_readStreamSession(
      sessionHandle, reinterpret_cast<tCANStreamMessage*>(messages),
      messagesToRead, messagesRead, status);
}
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  FRC_NetworkCommunication_CANSessionMux_getCANStatus(
      percentBusUtilization, busOffCount, txFullCount, receiveErrorCount,
      transmitErrorCount, status);
}
}  // extern "C"
