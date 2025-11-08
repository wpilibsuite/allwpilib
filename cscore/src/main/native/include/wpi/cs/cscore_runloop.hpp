// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace cs {

void RunMainRunLoop();

/**
 * Runs main run loop with timeout.
 *
 * @param timeout Timeout in seconds.
 */
int RunMainRunLoopTimeout(double timeout);

void StopMainRunLoop();

}  // namespace cs
