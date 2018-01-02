/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>
#include <memory>

#include <support/mutex.h>

#include "MockData/CanData.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {

typedef HalCallbackListenerVectorImpl<HAL_CAN_SendMessageCallback>
    CanSendMessageListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_CAN_ReceiveMessageCallback>
    CanReceiveMessageListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_CAN_OpenStreamSessionCallback>
    CanOpenStreamSessionListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_CAN_CloseStreamSessionCallback>
    CanCloseStreamSessionListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_CAN_ReadStreamSessionCallback>
    CanReadStreamSessionListenerVector;
typedef HalCallbackListenerVectorImpl<HAL_CAN_GetCANStatusCallback>
    CanGetCANStatusListenerVector;

class CanData {
 public:
  void ResetData();

  void SendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize,
                   int32_t periodMs, int32_t* status);
  void ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                      uint8_t* data, uint8_t* dataSize, uint32_t* timeStamp,
                      int32_t* status);
  void OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                         uint32_t messageIDMask, uint32_t maxMessages,
                         int32_t* status);
  void CloseStreamSession(uint32_t sessionHandle);
  void ReadStreamSession(uint32_t sessionHandle,
                         struct HAL_CANStreamMessage* messages,
                         uint32_t messagesToRead, uint32_t* messagesRead,
                         int32_t* status);
  void GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                    uint32_t* txFullCount, uint32_t* receiveErrorCount,
                    uint32_t* transmitErrorCount, int32_t* status);

  int32_t RegisterSendMessageCallback(HAL_CAN_SendMessageCallback callback,
                                      void* param);
  void CancelSendMessageCallback(int32_t uid);

  int32_t RegisterReceiveMessageCallback(
      HAL_CAN_ReceiveMessageCallback callback, void* param);
  void CancelReceiveMessageCallback(int32_t uid);

  int32_t RegisterOpenStreamCallback(HAL_CAN_OpenStreamSessionCallback callback,
                                     void* param);
  void CancelOpenStreamCallback(int32_t uid);

  int32_t RegisterCloseStreamCallback(
      HAL_CAN_CloseStreamSessionCallback callback, void* param);
  void CancelCloseStreamCallback(int32_t uid);

  int32_t RegisterReadStreamCallback(HAL_CAN_ReadStreamSessionCallback callback,
                                     void* param);
  void CancelReadStreamCallback(int32_t uid);

  int32_t RegisterGetCANStatusCallback(HAL_CAN_GetCANStatusCallback callback,
                                       void* param);
  void CancelGetCANStatusCallback(int32_t uid);

 protected:
  template <typename VectorType, typename CallbackType>
  int32_t RegisterCanCallback(CallbackType& callback,
                              std::shared_ptr<VectorType>& callbackVector,
                              const char* callbackName, void* param) {
    // Must return -1 on a null callback for error handling
    if (callback == nullptr) return -1;
    int32_t newUid = 0;
    {
      std::lock_guard<wpi::mutex> lock(m_registerMutex);
      callbackVector = RegisterCallbackImpl(callbackVector, callbackName,
                                            callback, param, &newUid);
    }
    return newUid;
  }

  wpi::mutex m_registerMutex;

  std::shared_ptr<CanSendMessageListenerVector> m_sendMessageCallback;
  std::shared_ptr<CanReceiveMessageListenerVector> m_receiveMessageCallback;
  std::shared_ptr<CanOpenStreamSessionListenerVector>
      m_openStreamSessionCallback;
  std::shared_ptr<CanCloseStreamSessionListenerVector>
      m_closeStreamSessionCallback;
  std::shared_ptr<CanReadStreamSessionListenerVector>
      m_readStreamSessionCallback;
  std::shared_ptr<CanGetCANStatusListenerVector> m_getCanStatusCallback;
};

extern CanData* SimCanData;

}  // namespace hal
