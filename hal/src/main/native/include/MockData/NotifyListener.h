/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL_Value.h"

typedef void (*HAL_NotifyCallback)(const char* name, void* param,
                                   const struct HAL_Value* value);

namespace hal {
struct NotifyListener {
  NotifyListener() = default;
  NotifyListener(void* param_, HAL_NotifyCallback callback_)
      : callback(callback_), param(param_) {}

  explicit operator bool() const { return callback != nullptr; }

  HAL_NotifyCallback callback;
  void* param;
};
}  // namespace hal
