/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  CallbackFunction callback;
  void* param;
};

}  // namespace hal

#endif
