/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <thread>

namespace frc {

int GetThreadPriority(std::thread& thread, bool* isRealTime);
int GetCurrentThreadPriority(bool* isRealTime);
bool SetThreadPriority(std::thread& thread, bool realTime, int priority);
bool SetCurrentThreadPriority(bool realTime, int priority);

}  // namespace frc
