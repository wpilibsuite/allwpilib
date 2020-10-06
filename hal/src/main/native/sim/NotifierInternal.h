/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace hal {
void PauseNotifiers();
void ResumeNotifiers();
void WakeupNotifiers();
void WaitNotifiers();
void WakeupWaitNotifiers();
}  // namespace hal
