/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "cscore.h"
#include <thread>

int main() {
  std::cout << cs::GetHostname() << std::endl;

  CS_Status status = 0;
  auto cam = cs::CreateUsbCameraDev("Camera", 0, &status);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  //cs::ReleaseSource(cam, &status);
}
