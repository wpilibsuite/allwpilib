/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <thread>

#include <hal/HALBase.h>

int main() {
  HAL_Initialize(500, 0);
  std::this_thread::sleep_for(std::chrono::seconds(2));
}
