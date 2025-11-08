// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/cscore_runloop.hpp"

#include <CoreFoundation/CFRunLoop.h>
#import <Foundation/Foundation.h>

namespace cs {

void RunMainRunLoop() {
  if (CFRunLoopGetMain() != CFRunLoopGetCurrent()) {
    NSLog(@"This method can only be called from the main thread");
    return;
  }
  CFRunLoopRun();
}

int RunMainRunLoopTimeout(double timeout) {
  if (CFRunLoopGetMain() != CFRunLoopGetCurrent()) {
    NSLog(@"This method can only be called from the main thread");
    return -1;
  }
  return CFRunLoopRunInMode(kCFRunLoopDefaultMode, timeout, false);
}

void StopMainRunLoop() {
  CFRunLoopStop(CFRunLoopGetMain());
}

}
