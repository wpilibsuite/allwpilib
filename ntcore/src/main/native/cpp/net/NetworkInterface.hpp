// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "MessageHandler.h"

namespace nt::net {

class ILocalStorage : public ServerMessageHandler {
 public:
  virtual void StartNetwork(ClientMessageHandler* network) = 0;
  virtual void ClearNetwork() = 0;
};

}  // namespace nt::net
