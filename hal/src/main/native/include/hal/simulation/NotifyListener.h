// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Value.h"

typedef void (*HAL_NotifyCallback)(const char* name, void* param,
                                   const struct HAL_Value* value);

typedef void (*HAL_BufferCallback)(const char* name, void* param,
                                   unsigned char* buffer, unsigned int count);

typedef void (*HAL_ConstBufferCallback)(const char* name, void* param,
                                        const unsigned char* buffer,
                                        unsigned int count);

#ifdef __cplusplus

namespace hal {

template <typename CallbackFunction>
struct HalCallbackListener {
  HalCallbackListener() = default;
  HalCallbackListener(void* param_, CallbackFunction callback_)
      : callback(callback_), param(param_) {}

  explicit operator bool() const { return callback != nullptr; }

  CallbackFunction callback = nullptr;
  void* param = nullptr;
};

}  // namespace hal

#endif
