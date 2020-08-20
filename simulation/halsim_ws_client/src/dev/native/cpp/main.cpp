/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_RunMain();

  int cycleCount = 0;
  while (cycleCount < 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cycleCount++;
    std::cout << "Count: " << cycleCount << std::endl;
  }

  std::cout << "DONE" << std::endl;
  HAL_ExitMain();
}
