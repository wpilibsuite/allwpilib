// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <hal/Value.h>
#include <wpi/StringRef.h>

namespace frc::sim {

using NotifyCallback = std::function<void(wpi::StringRef, const HAL_Value*)>;
using ConstBufferCallback = std::function<void(
    wpi::StringRef, const unsigned char* buffer, unsigned int count)>;
using CancelCallbackFunc = void (*)(int32_t index, int32_t uid);
using CancelCallbackNoIndexFunc = void (*)(int32_t uid);
using CancelCallbackChannelFunc = void (*)(int32_t index, int32_t channel,
                                           int32_t uid);

void CallbackStoreThunk(const char* name, void* param, const HAL_Value* value);
void ConstBufferCallbackStoreThunk(const char* name, void* param,
                                   const unsigned char* buffer,
                                   unsigned int count);

/**
 * Manages simulation callbacks; each object is associated with a callback.
 */
class CallbackStore {
 public:
  CallbackStore(int32_t i, NotifyCallback cb, CancelCallbackNoIndexFunc ccf);

  CallbackStore(int32_t i, int32_t u, NotifyCallback cb,
                CancelCallbackFunc ccf);

  CallbackStore(int32_t i, int32_t c, int32_t u, NotifyCallback cb,
                CancelCallbackChannelFunc ccf);

  CallbackStore(int32_t i, ConstBufferCallback cb,
                CancelCallbackNoIndexFunc ccf);

  CallbackStore(int32_t i, int32_t u, ConstBufferCallback cb,
                CancelCallbackFunc ccf);

  CallbackStore(int32_t i, int32_t c, int32_t u, ConstBufferCallback cb,
                CancelCallbackChannelFunc ccf);

  CallbackStore(const CallbackStore&) = delete;
  CallbackStore& operator=(const CallbackStore&) = delete;

  ~CallbackStore();

  void SetUid(int32_t uid);

  friend void CallbackStoreThunk(const char* name, void* param,
                                 const HAL_Value* value);

  friend void ConstBufferCallbackStoreThunk(const char* name, void* param,
                                            const unsigned char* buffer,
                                            unsigned int count);

 private:
  int32_t index;
  int32_t channel;
  int32_t uid;

  NotifyCallback callback;
  ConstBufferCallback constBufferCallback;
  union {
    CancelCallbackFunc ccf;
    CancelCallbackChannelFunc cccf;
    CancelCallbackNoIndexFunc ccnif;
  };
  enum CancelType { Normal, Channel, NoIndex };
  CancelType cancelType;
};
}  // namespace frc::sim
