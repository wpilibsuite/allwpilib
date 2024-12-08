// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/Synchronization.h>

#include "cscore_runloop.h"

static wpi::Event& GetInstance() {
  static wpi::Event event;
  return event;
}

namespace cs {

void RunMainRunLoop() {
  wpi::Event& event = GetInstance();
  wpi::WaitForObject(event.GetHandle());
}

int RunMainRunLoopTimeout(double timeout) {
  wpi::Event& event = GetInstance();
  bool timedOut = false;
  bool signaled = wpi::WaitForObject(event.GetHandle(), timeout, &timedOut);
  if (timedOut) {
    return 3;
  }
  if (signaled) {
    return 2;
  }
  return 1;
}

void StopMainRunLoop() {
  wpi::Event& event = GetInstance();
  event.Set();
}

}  // namespace cs
