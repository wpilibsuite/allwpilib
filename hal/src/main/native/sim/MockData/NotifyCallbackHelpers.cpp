/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

std::shared_ptr<NotifyListenerVector> RegisterCallback(
    std::shared_ptr<NotifyListenerVector> currentVector, const char* name,
    HAL_NotifyCallback callback, void* param, int32_t* newUid) {
  return RegisterCallbackImpl<NotifyListenerVector, HAL_NotifyCallback>(
      currentVector, name, callback, param, newUid);
}

std::shared_ptr<NotifyListenerVector> CancelCallback(
    std::shared_ptr<NotifyListenerVector> currentVector, int32_t uid) {
  return CancelCallbackImpl<NotifyListenerVector, HAL_NotifyCallback>(
      currentVector, uid);
}

void InvokeCallback(std::shared_ptr<NotifyListenerVector> currentVector,
                    const char* name, const HAL_Value* value) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, value);
  }
}

std::shared_ptr<BufferListenerVector> RegisterCallback(
    std::shared_ptr<BufferListenerVector> currentVector, const char* name,
    HAL_BufferCallback callback, void* param, int32_t* newUid) {
  return RegisterCallbackImpl<BufferListenerVector, HAL_BufferCallback>(
      currentVector, name, callback, param, newUid);
}

std::shared_ptr<BufferListenerVector> CancelCallback(
    std::shared_ptr<BufferListenerVector> currentVector, int32_t uid) {
  return CancelCallbackImpl<BufferListenerVector, HAL_BufferCallback>(
      currentVector, uid);
}

void InvokeCallback(std::shared_ptr<BufferListenerVector> currentVector,
                    const char* name, uint8_t* buffer, int32_t count) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, buffer, count);
  }
}

std::shared_ptr<ConstBufferListenerVector> RegisterCallback(
    std::shared_ptr<ConstBufferListenerVector> currentVector, const char* name,
    HAL_ConstBufferCallback callback, void* param, int32_t* newUid) {
  return RegisterCallbackImpl<ConstBufferListenerVector,
                              HAL_ConstBufferCallback>(currentVector, name,
                                                       callback, param, newUid);
}

std::shared_ptr<ConstBufferListenerVector> CancelCallback(
    std::shared_ptr<ConstBufferListenerVector> currentVector, int32_t uid) {
  return CancelCallbackImpl<ConstBufferListenerVector, HAL_ConstBufferCallback>(
      currentVector, uid);
}

void InvokeCallback(std::shared_ptr<ConstBufferListenerVector> currentVector,
                    const char* name, const uint8_t* buffer, int32_t count) {
  // Return if no callbacks are assigned
  if (currentVector == nullptr) return;
  // Get a copy of the shared_ptr, then iterate and callback listeners
  auto newCallbacks = currentVector;
  for (size_t i = 0; i < newCallbacks->size(); ++i) {
    if (!(*newCallbacks)[i]) continue;  // removed
    auto listener = (*newCallbacks)[i];
    listener.callback(name, listener.param, buffer, count);
  }
}
