/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <wpi/StringRef.h>

#include "hal/Value.h"

namespace frc {
namespace sim {

using NotifyCallback = std::function<void(wpi::StringRef, const HAL_Value*)>;
typedef void (*CancelCallbackFunc)(int32_t index, int32_t uid);
typedef void (*CancelCallbackNoIndexFunc)(int32_t uid);
typedef void (*CancelCallbackChannelFunc)(int32_t index, int32_t channel,
                                          int32_t uid);

void CallbackStoreThunk(const char* name, void* param, const HAL_Value* value);

class CallbackStore {
 public:
  CallbackStore(int32_t i, NotifyCallback cb, CancelCallbackNoIndexFunc ccf) {
    index = i;
    callback = cb;
    this->ccnif = ccf;
    cancelType = NoIndex;
  }

  CallbackStore(int32_t i, int32_t u, NotifyCallback cb,
                CancelCallbackFunc ccf) {
    index = i;
    uid = u;
    callback = cb;
    this->ccf = ccf;
    cancelType = Normal;
  }

  CallbackStore(int32_t i, int32_t c, int32_t u, NotifyCallback cb,
                CancelCallbackChannelFunc ccf) {
    index = i;
    channel = c;
    uid = u;
    callback = cb;
    this->cccf = ccf;
    cancelType = Channel;
  }

  ~CallbackStore() {
    switch (cancelType) {
      case Normal:
        ccf(index, uid);
        break;
      case Channel:
        cccf(index, channel, uid);
        break;
      case NoIndex:
        ccnif(uid);
        break;
    }
  }

  void SetUid(int32_t uid) { this->uid = uid; }

  friend void CallbackStoreThunk(const char* name, void* param,
                                 const HAL_Value* value);

 private:
  int32_t index;
  int32_t channel;
  int32_t uid;

  NotifyCallback callback;
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
