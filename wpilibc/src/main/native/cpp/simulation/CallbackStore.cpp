// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/CallbackStore.h"

#include <utility>

using namespace frc;
using namespace frc::sim;

void frc::sim::CallbackStoreThunk(const char* name, void* param,
                                  const HAL_Value* value) {
  reinterpret_cast<CallbackStore*>(param)->callback(name, value);
}

void frc::sim::ConstBufferCallbackStoreThunk(const char* name, void* param,
                                             const unsigned char* buffer,
                                             unsigned int count) {
  reinterpret_cast<CallbackStore*>(param)->constBufferCallback(name, buffer,
                                                               count);
}

CallbackStore::CallbackStore(int32_t i, NotifyCallback cb,
                             CancelCallbackNoIndexFunc ccf)
    : index(i), callback(std::move(cb)), cancelType(NoIndex) {
  this->ccnif = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t u, NotifyCallback cb,
                             CancelCallbackFunc ccf)
    : index(i), uid(u), callback(std::move(cb)), cancelType(Normal) {
  this->ccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t c, int32_t u, NotifyCallback cb,
                             CancelCallbackChannelFunc ccf)
    : index(i),
      channel(c),
      uid(u),
      callback(std::move(cb)),
      cancelType(Channel) {
  this->cccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, ConstBufferCallback cb,
                             CancelCallbackNoIndexFunc ccf)
    : index(i), constBufferCallback(std::move(cb)), cancelType(NoIndex) {
  this->ccnif = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t u, ConstBufferCallback cb,
                             CancelCallbackFunc ccf)
    : index(i), uid(u), constBufferCallback(std::move(cb)), cancelType(Normal) {
  this->ccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t c, int32_t u,
                             ConstBufferCallback cb,
                             CancelCallbackChannelFunc ccf)
    : index(i),
      channel(c),
      uid(u),
      constBufferCallback(std::move(cb)),
      cancelType(Channel) {
  this->cccf = ccf;
}

CallbackStore::~CallbackStore() {
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

void CallbackStore::SetUid(int32_t uid) {
  this->uid = uid;
}
