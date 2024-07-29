// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>
#include <wpi/print.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_RunMain();

  int cycleCount = 0;
  while (cycleCount < 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cycleCount++;
    wpi::print("Count: {}\n", cycleCount);
  }

  wpi::print("DONE\n");
  HAL_ExitMain();
}
