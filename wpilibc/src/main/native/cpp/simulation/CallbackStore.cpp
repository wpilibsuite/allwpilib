/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/CallbackStore.h"

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
    : index(i), callback(cb), cancelType(NoIndex) {
  this->ccnif = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t u, NotifyCallback cb,
                             CancelCallbackFunc ccf)
    : index(i), uid(u), callback(cb), cancelType(Normal) {
  this->ccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t c, int32_t u, NotifyCallback cb,
                             CancelCallbackChannelFunc ccf)
    : index(i), channel(c), uid(u), callback(cb), cancelType(Channel) {
  this->cccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, ConstBufferCallback cb,
                             CancelCallbackNoIndexFunc ccf)
    : index(i), constBufferCallback(cb), cancelType(NoIndex) {
  this->ccnif = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t u, ConstBufferCallback cb,
                             CancelCallbackFunc ccf)
    : index(i), uid(u), constBufferCallback(cb), cancelType(Normal) {
  this->ccf = ccf;
}

CallbackStore::CallbackStore(int32_t i, int32_t c, int32_t u,
                             ConstBufferCallback cb,
                             CancelCallbackChannelFunc ccf)
    : index(i),
      channel(c),
      uid(u),
      constBufferCallback(cb),
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

void CallbackStore::SetUid(int32_t uid) { this->uid = uid; }
