// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace hal {
void PauseNotifiers();
void ResumeNotifiers();
void WakeupNotifiers();
void WaitNotifiers();
void WakeupWaitNotifiers();
}  // namespace hal
