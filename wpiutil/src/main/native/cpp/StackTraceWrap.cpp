// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <string>

#include "wpi/StackTrace.h"

static std::atomic<std::string (*)(int offset)> gStackTraceImpl{
    wpi::GetStackTraceDefault};

std::string wpi::GetStackTrace(int offset) {
  return (gStackTraceImpl.load())(offset);
}

void wpi::SetGetStackTraceImpl(std::string (*func)(int offset)) {
  gStackTraceImpl = func ? func : wpi::GetStackTraceDefault;
}
