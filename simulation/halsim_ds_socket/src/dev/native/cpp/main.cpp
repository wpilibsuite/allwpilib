/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <wpi/Format.h>
#include <wpi/raw_ostream.h>

extern "C" int HALSIM_InitExtension(void);

int main() {
  HAL_Initialize(500, 0);
  HALSIM_InitExtension();

  HAL_ObserveUserProgramStarting();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
