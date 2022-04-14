// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_ObserveUserProgramStarting();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
