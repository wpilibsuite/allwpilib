// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/cscore_runloop.hpp"
#include "wpi/util/Synchronization.h"

static wpi::util::Event& GetInstance() {
  static wpi::util::Event event;
  return event;
}

namespace wpi::cs {

void RunMainRunLoop() {
  wpi::util::Event& event = GetInstance();
  wpi::util::WaitForObject(event.GetHandle());
}

int RunMainRunLoopTimeout(double timeout) {
  wpi::util::Event& event = GetInstance();
  bool timedOut = false;
  bool signaled = wpi::util::WaitForObject(event.GetHandle(), timeout, &timedOut);
  if (timedOut) {
    return 3;
  }
  if (signaled) {
    return 2;
  }
  return 1;
}

void StopMainRunLoop() {
  wpi::util::Event& event = GetInstance();
  event.Set();
}

}  // namespace wpi::cs
