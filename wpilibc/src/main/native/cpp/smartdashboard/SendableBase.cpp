// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/SendableBase.h"

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

SendableBase::SendableBase(bool addLiveWindow) {
  if (addLiveWindow) {
    SendableRegistry::GetInstance().AddLW(this, "");
  } else {
    SendableRegistry::GetInstance().Add(this, "");
  }
}
