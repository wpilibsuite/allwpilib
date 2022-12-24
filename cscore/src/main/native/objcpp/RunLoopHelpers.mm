// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_runloop.h"

#include <CoreFoundation/CFRunLoop.h>

namespace cs {
void RunOsxRunLoop() {
  CFRunLoopRun();
}

int RunOsxRunLoopTimeout(double timeoutSeconds) {
  return CFRunLoopRunInMode(kCFRunLoopDefaultMode, timeoutSeconds, false);
}

void StopOsxMainRunLoop() {
  CFRunLoopStop(CFRunLoopGetMain());
}
}
