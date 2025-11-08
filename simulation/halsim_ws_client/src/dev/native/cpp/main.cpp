// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Main.h"
#include "wpi/util/print.hpp"

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_RunMain();

  int cycleCount = 0;
  while (cycleCount < 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cycleCount++;
    wpi::util::print("Count: {}\n", cycleCount);
  }

  wpi::util::print("DONE\n");
  HAL_ExitMain();
}
