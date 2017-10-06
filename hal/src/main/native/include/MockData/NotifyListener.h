/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL_Value.h"

typedef void (*HAL_NotifyCallback)(const char* name, void* param,
                                   const struct HAL_Value* value);

typedef void (*HAL_ReadBufferCallback)(const char* name, void* param,
                                       unsigned char* buffer, unsigned int count);

typedef void (*HAL_WriteBufferCallback)(const char* name, void* param,
                                        unsigned char* buffer, unsigned int count);

namespace hal {

template<typename CallbackFunction>
struct HalCallbackListener {
	HalCallbackListener() = default;
	HalCallbackListener(void* param_, CallbackFunction callback_)
      : callback(callback_), param(param_) {}

  explicit operator bool() const { return callback != nullptr; }

  CallbackFunction callback;
  void* param;
};

//typedef HalCallbackListener<HAL_NotifyCallback> NotifyListener;
//typedef HalCallbackListener<HAL_ReadBufferCallback> ReadBufferListener;
//typedef HalCallbackListener<HAL_WriteBufferCallback> WriteBufferListener;
}  // namespace hal
