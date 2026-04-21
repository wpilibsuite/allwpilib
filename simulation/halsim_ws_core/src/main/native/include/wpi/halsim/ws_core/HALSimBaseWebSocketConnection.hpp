// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpilibws {

class HALSimBaseWebSocketConnection {
 public:
  virtual void OnSimValueChanged(const wpi::util::json& msg) = 0;

 protected:
  virtual ~HALSimBaseWebSocketConnection() = default;
};

}  // namespace wpilibws
