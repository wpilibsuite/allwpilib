/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <hal/Value.h>
#include <wpi/StringRef.h>

namespace frc {
namespace sim {

using NotifyCallback = std::function<void(wpi::StringRef, const HAL_Value*)>;
using ConstBufferCallback = std::function<void(
    wpi::StringRef, const unsigned char* buffer, unsigned int count)>;
typedef void (*CancelCallbackFunc)(int32_t index, int32_t uid);
typedef void (*CancelCallbackNoIndexFunc)(int32_t uid);
typedef void (*CancelCallbackChannelFunc)(int32_t index, int32_t channel,
                                          int32_t uid);

void CallbackStoreThunk(const char* name, void* param, const HAL_Value* value);
void ConstBufferCallbackStoreThunk(const char* name, void* param,
                                   const unsigned char* buffer,
                                   unsigned int count);

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
}  // namespace sim
}  // namespace frc
