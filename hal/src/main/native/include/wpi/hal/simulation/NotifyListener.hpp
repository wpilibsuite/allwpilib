// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/simulation/NotifyListener.h"

namespace wpi::hal {

template <typename CallbackFunction>
struct HalCallbackListener {
  HalCallbackListener() = default;
  HalCallbackListener(void* param_, CallbackFunction callback_)
      : callback(callback_), param(param_) {}

  explicit operator bool() const { return callback != nullptr; }

  CallbackFunction callback = nullptr;
  void* param = nullptr;
};

}  // namespace wpi::hal
