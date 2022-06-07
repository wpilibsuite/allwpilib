// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MulticastHandleManager.h"

using namespace wpi;

MulticastHandleManager& wpi::GetMulticastManager() {
  static MulticastHandleManager manager;
  return manager;
}
