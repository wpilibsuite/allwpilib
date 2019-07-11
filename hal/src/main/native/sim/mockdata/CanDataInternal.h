/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/CanData.h"
#include "mockdata/SimCallbackRegistry.h"

namespace hal {

class CanData {
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(SendMessage)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(ReceiveMessage)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(OpenStream)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(CloseStream)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(ReadStream)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(GetCanStatus)

 public:
  SimCallbackRegistry<HAL_CAN_SendMessageCallback, GetSendMessageName>
      sendMessage;
  SimCallbackRegistry<HAL_CAN_ReceiveMessageCallback, GetReceiveMessageName>
      receiveMessage;
  SimCallbackRegistry<HAL_CAN_OpenStreamSessionCallback, GetOpenStreamName>
      openStreamSession;
  SimCallbackRegistry<HAL_CAN_CloseStreamSessionCallback, GetCloseStreamName>
      closeStreamSession;
  SimCallbackRegistry<HAL_CAN_ReadStreamSessionCallback, GetReadStreamName>
      readStreamSession;
  SimCallbackRegistry<HAL_CAN_GetCANStatusCallback, GetGetCanStatusName>
      getCANStatus;

  void ResetData();
};

extern CanData* SimCanData;

}  // namespace hal
