/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

enum class AnalogTriggerType {
  kInWindow = 0,
  kState = 1,
  kRisingPulse = 2,
  kFallingPulse = 3
};

}  // namespace frc
