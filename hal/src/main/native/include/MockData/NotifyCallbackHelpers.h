/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include <memory>

#include "MockData/NotifyListenerVector.h"

template <typename VectorType, typename CallbackType>
std::shared_ptr<VectorType> RegisterCallbackImpl(
    std::shared_ptr<VectorType> currentVector, const char* name,
    CallbackType callback, void* param, int32_t* newUid) {
  std::shared_ptr<VectorType> newCallbacks;
  if (currentVector == nullptr) {
    newCallbacks = std::make_shared<VectorType>(
        param, callback, reinterpret_cast<unsigned int*>(newUid));
  } else {
    newCallbacks = currentVector->emplace_back(
        param, callback, reinterpret_cast<unsigned int*>(newUid));
  }
  return newCallbacks;
}

template <typename VectorType, typename CallbackType>
std::shared_ptr<VectorType> CancelCallbackImpl(
    std::shared_ptr<VectorType> currentVector, int32_t uid) {
  // Create a copy of the callbacks to erase from
  auto newCallbacks = currentVector->erase(uid);
  return newCallbacks;
}

std::shared_ptr<hal::NotifyListenerVector> RegisterCallback(
    std::shared_ptr<hal::NotifyListenerVector> currentVector, const char* name,
    HAL_NotifyCallback callback, void* param, int32_t* newUid);

std::shared_ptr<hal::NotifyListenerVector> CancelCallback(
    std::shared_ptr<hal::NotifyListenerVector> currentVector, int32_t uid);

void InvokeCallback(std::shared_ptr<hal::NotifyListenerVector> currentVector,
                    const char* name, const HAL_Value* value);

std::shared_ptr<hal::BufferListenerVector> RegisterCallback(
    std::shared_ptr<hal::BufferListenerVector> currentVector, const char* name,
    HAL_BufferCallback callback, void* param, int32_t* newUid);

std::shared_ptr<hal::BufferListenerVector> CancelCallback(
    std::shared_ptr<hal::BufferListenerVector> currentVector, int32_t uid);

void InvokeCallback(std::shared_ptr<hal::BufferListenerVector> currentVector,
                    const char* name, uint8_t* buffer, int32_t count);

std::shared_ptr<hal::ConstBufferListenerVector> RegisterCallback(
    std::shared_ptr<hal::ConstBufferListenerVector> currentVector,
    const char* name, HAL_ConstBufferCallback callback, void* param,
    int32_t* newUid);

std::shared_ptr<hal::ConstBufferListenerVector> CancelCallback(
    std::shared_ptr<hal::ConstBufferListenerVector> currentVector, int32_t uid);

void InvokeCallback(
    std::shared_ptr<hal::ConstBufferListenerVector> currentVector,
    const char* name, const uint8_t* buffer, int32_t count);

#endif
