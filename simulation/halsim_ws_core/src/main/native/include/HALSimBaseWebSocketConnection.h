/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/json.h>

namespace wpilibws {

class HALSimBaseWebSocketConnection {
 public:
  virtual void OnSimValueChanged(const wpi::json& msg) = 0;

 protected:
  virtual ~HALSimBaseWebSocketConnection() = default;
};

}  // namespace wpilibws
