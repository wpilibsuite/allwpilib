/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableBase.h"

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

SendableBase::SendableBase(bool addLiveWindow) {
  if (addLiveWindow)
    SendableRegistry::GetInstance().AddLW(this, "");
  else
    SendableRegistry::GetInstance().Add(this, "");
}
