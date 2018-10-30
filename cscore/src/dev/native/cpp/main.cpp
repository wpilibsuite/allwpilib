/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "cscore.h"

int main() {
  std::cout << cs::GetHostname() << std::endl;
  cs::UsbCamera camera{"camera", 0};
  cs::MjpegServer server{"server", 1181};
  camera.SetBrightness(0);
  server.SetSource(camera);

  system("PAUSE");

  camera.SetVideoMode(cs::VideoMode::kYUYV, 1280, 720, 30);

  system("PAUSE");
  }
