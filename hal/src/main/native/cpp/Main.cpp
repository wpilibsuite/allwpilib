/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Main.h"

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

static void DefaultMain();
static void DefaultExit();

static bool gHasMain = false;
static void (*gMainFunc)() = DefaultMain;
static void (*gExitFunc)() = DefaultExit;
static bool gExited = false;
static wpi::mutex gExitMutex;
static wpi::condition_variable gExitCv;

static void DefaultMain() {
  std::unique_lock lock{gExitMutex};
  gExitCv.wait(lock, [] { return gExited; });
}

static void DefaultExit() {
  std::lock_guard lock{gExitMutex};
  gExited = true;
  gExitCv.notify_all();
}

extern "C" {

void HAL_SetMain(void (*mainFunc)(void), void (*exitFunc)(void)) {
  gHasMain = true;
  gMainFunc = mainFunc;
  gExitFunc = exitFunc;
}

HAL_Bool HAL_HasMain(void) { return gHasMain; }

void HAL_RunMain(void) { gMainFunc(); }

void HAL_ExitMain(void) { gExitFunc(); }

}  // extern "C"
