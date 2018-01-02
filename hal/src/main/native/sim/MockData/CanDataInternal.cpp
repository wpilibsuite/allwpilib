/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CanDataInternal.h"

#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeCanData() {
  static CanData scd;
  ::hal::SimCanData = &scd;
}
}  // namespace init
}  // namespace hal

CanData* hal::SimCanData;
void InvokeCallback(std::shared_ptr<CanSendMessageListenerVector> currentVector,
                    const char* name, uint32_t messageID, const uint8_t* data,
                    uint8_t dataSize, int32_t periodMs, int32_t* status) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, messageID, data, dataSize, periodMs,
                      status);
  }
}

void InvokeCallback(
    std::shared_ptr<CanReceiveMessageListenerVector> currentVector,
    const char* name, uint32_t* messageID, uint32_t messageIDMask,
    uint8_t* data, uint8_t* dataSize, uint32_t* timeStamp, int32_t* status) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, messageID, messageIDMask, data,
                      dataSize, timeStamp, status);
  }
}

void InvokeCallback(
    std::shared_ptr<CanOpenStreamSessionListenerVector> currentVector,
    const char* name, uint32_t* sessionHandle, uint32_t messageID,
    uint32_t messageIDMask, uint32_t maxMessages, int32_t* status) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, sessionHandle, messageID,
                      messageIDMask, maxMessages, status);
  }
}

void InvokeCallback(
    std::shared_ptr<CanCloseStreamSessionListenerVector> currentVector,
    const char* name, uint32_t sessionHandle) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, sessionHandle);
  }
}

void InvokeCallback(
    std::shared_ptr<CanReadStreamSessionListenerVector> currentVector,
    const char* name, uint32_t sessionHandle,
    struct HAL_CANStreamMessage* messages, uint32_t messagesToRead,
    uint32_t* messagesRead, int32_t* status) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, sessionHandle, messages,
                      messagesToRead, messagesRead, status);
  }
}

void InvokeCallback(
    std::shared_ptr<CanGetCANStatusListenerVector> currentVector,
    const char* name, float* percentBusUtilization, uint32_t* busOffCount,
    uint32_t* txFullCount, uint32_t* receiveErrorCount,
    uint32_t* transmitErrorCount, int32_t* status) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, percentBusUtilization, busOffCount,
                      txFullCount, receiveErrorCount, transmitErrorCount,
                      status);
  }
}

void CanData::ResetData() {
  m_sendMessageCallback = nullptr;
  m_receiveMessageCallback = nullptr;
  m_openStreamSessionCallback = nullptr;
  m_closeStreamSessionCallback = nullptr;
  m_readStreamSessionCallback = nullptr;
  m_getCanStatusCallback = nullptr;
}

void CanData::SendMessage(uint32_t messageID, const uint8_t* data,
                          uint8_t dataSize, int32_t periodMs, int32_t* status) {
  InvokeCallback(m_sendMessageCallback, "SendMessage", messageID, data,
                 dataSize, periodMs, status);
}

void CanData::ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                             uint8_t* data, uint8_t* dataSize,
                             uint32_t* timeStamp, int32_t* status) {
  InvokeCallback(m_receiveMessageCallback, "ReceiveMessage", messageID,
                 messageIDMask, data, dataSize, timeStamp, status);
}

void CanData::OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                                uint32_t messageIDMask, uint32_t maxMessages,
                                int32_t* status) {
  InvokeCallback(m_openStreamSessionCallback, "OpenStream", sessionHandle,
                 messageID, messageIDMask, maxMessages, status);
}

void CanData::CloseStreamSession(uint32_t sessionHandle) {
  InvokeCallback(m_closeStreamSessionCallback, "CloseStream", sessionHandle);
}

void CanData::ReadStreamSession(uint32_t sessionHandle,
                                struct HAL_CANStreamMessage* messages,
                                uint32_t messagesToRead, uint32_t* messagesRead,
                                int32_t* status) {
  InvokeCallback(m_readStreamSessionCallback, "ReadStream", sessionHandle,
                 messages, messagesToRead, messagesRead, status);
}

void CanData::GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                           uint32_t* txFullCount, uint32_t* receiveErrorCount,
                           uint32_t* transmitErrorCount, int32_t* status) {
  InvokeCallback(m_getCanStatusCallback, "GetCanStatus", percentBusUtilization,
                 busOffCount, txFullCount, receiveErrorCount,
                 transmitErrorCount, status);
}

int32_t CanData::RegisterSendMessageCallback(
    HAL_CAN_SendMessageCallback callback, void* param) {
  return RegisterCanCallback<CanSendMessageListenerVector,
                             HAL_CAN_SendMessageCallback>(
      callback, m_sendMessageCallback, "SendMessage", param);
}
void CanData::CancelSendMessageCallback(int32_t uid) {
  m_sendMessageCallback = CancelCallbackImpl<CanSendMessageListenerVector,
                                             HAL_CAN_SendMessageCallback>(
      m_sendMessageCallback, uid);
}

int32_t CanData::RegisterReceiveMessageCallback(
    HAL_CAN_ReceiveMessageCallback callback, void* param) {
  return RegisterCanCallback<CanReceiveMessageListenerVector,
                             HAL_CAN_ReceiveMessageCallback>(
      callback, m_receiveMessageCallback, "ReceiveMessage", param);
}
void CanData::CancelReceiveMessageCallback(int32_t uid) {
  m_receiveMessageCallback = CancelCallbackImpl<CanReceiveMessageListenerVector,
                                                HAL_CAN_ReceiveMessageCallback>(
      m_receiveMessageCallback, uid);
}

int32_t CanData::RegisterOpenStreamCallback(
    HAL_CAN_OpenStreamSessionCallback callback, void* param) {
  return RegisterCanCallback<CanOpenStreamSessionListenerVector,
                             HAL_CAN_OpenStreamSessionCallback>(
      callback, m_openStreamSessionCallback, "OpenStream", param);
}
void CanData::CancelOpenStreamCallback(int32_t uid) {
  m_openStreamSessionCallback =
      CancelCallbackImpl<CanOpenStreamSessionListenerVector,
                         HAL_CAN_OpenStreamSessionCallback>(
          m_openStreamSessionCallback, uid);
}

int32_t CanData::RegisterCloseStreamCallback(
    HAL_CAN_CloseStreamSessionCallback callback, void* param) {
  return RegisterCanCallback<CanCloseStreamSessionListenerVector,
                             HAL_CAN_CloseStreamSessionCallback>(
      callback, m_closeStreamSessionCallback, "CloseStream", param);
}
void CanData::CancelCloseStreamCallback(int32_t uid) {
  m_closeStreamSessionCallback =
      CancelCallbackImpl<CanCloseStreamSessionListenerVector,
                         HAL_CAN_CloseStreamSessionCallback>(
          m_closeStreamSessionCallback, uid);
}

int32_t CanData::RegisterReadStreamCallback(
    HAL_CAN_ReadStreamSessionCallback callback, void* param) {
  return RegisterCanCallback<CanReadStreamSessionListenerVector,
                             HAL_CAN_ReadStreamSessionCallback>(
      callback, m_readStreamSessionCallback, "ReadStream", param);
}
void CanData::CancelReadStreamCallback(int32_t uid) {
  m_readStreamSessionCallback =
      CancelCallbackImpl<CanReadStreamSessionListenerVector,
                         HAL_CAN_ReadStreamSessionCallback>(
          m_readStreamSessionCallback, uid);
}
int32_t CanData::RegisterGetCANStatusCallback(
    HAL_CAN_GetCANStatusCallback callback, void* param) {
  return RegisterCanCallback<CanGetCANStatusListenerVector,
                             HAL_CAN_GetCANStatusCallback>(
      callback, m_getCanStatusCallback, "GetCANStatus", param);
}
void CanData::CancelGetCANStatusCallback(int32_t uid) {
  m_getCanStatusCallback =
      CancelCallbackImpl<CanGetCANStatusListenerVector,
                         HAL_CAN_ReadStreamSessionCallback>(
          m_getCanStatusCallback, uid);
}

extern "C" {

void HALSIM_ResetCanData(void) { SimCanData->ResetData(); }

int32_t HALSIM_RegisterCanSendMessageCallback(
    HAL_CAN_SendMessageCallback callback, void* param) {
  return SimCanData->RegisterSendMessageCallback(callback, param);
}
void HALSIM_CancelCanSendMessageCallback(int32_t uid) {
  SimCanData->CancelSendMessageCallback(uid);
}

int32_t HALSIM_RegisterCanReceiveMessageCallback(
    HAL_CAN_ReceiveMessageCallback callback, void* param) {
  return SimCanData->RegisterReceiveMessageCallback(callback, param);
}
void HALSIM_CancelCanReceiveMessageCallback(int32_t uid) {
  SimCanData->CancelReceiveMessageCallback(uid);
}

int32_t HALSIM_RegisterCanOpenStreamCallback(
    HAL_CAN_OpenStreamSessionCallback callback, void* param) {
  return SimCanData->RegisterOpenStreamCallback(callback, param);
}
void HALSIM_CancelCanOpenStreamCallback(int32_t uid) {
  SimCanData->CancelOpenStreamCallback(uid);
}

int32_t HALSIM_RegisterCanCloseStreamCallback(
    HAL_CAN_CloseStreamSessionCallback callback, void* param) {
  return SimCanData->RegisterCloseStreamCallback(callback, param);
}
void HALSIM_CancelCanCloseStreamCallback(int32_t uid) {
  SimCanData->CancelCloseStreamCallback(uid);
}

int32_t HALSIM_RegisterCanReadStreamCallback(
    HAL_CAN_ReadStreamSessionCallback callback, void* param) {
  return SimCanData->RegisterReadStreamCallback(callback, param);
}
void HALSIM_CancelCanReadStreamCallback(int32_t uid) {
  SimCanData->CancelReadStreamCallback(uid);
}

int32_t HALSIM_RegisterCanGetCANStatusCallback(
    HAL_CAN_GetCANStatusCallback callback, void* param) {
  return SimCanData->RegisterGetCANStatusCallback(callback, param);
}
void HALSIM_CancelCanGetCANStatusCallback(int32_t uid) {
  SimCanData->CancelGetCANStatusCallback(uid);
}

}  // extern "C"
