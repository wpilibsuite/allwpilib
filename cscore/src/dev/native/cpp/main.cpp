// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>
#include <iostream>
#include <thread>
#include <CoreFoundation/CFRunLoop.h>

#include "cscore.h"

int main() {
  fmt::print("{}\n", cs::GetHostname());

    CS_Status status = 0;
  auto cams = cs::EnumerateUsbCameras(&status);
  for (auto&& cam : cams) {
    std::cout << cam.name << std::endl;
    std::cout << cam.path << std::endl;
  }


  CS_Source source = cs::CreateUsbCameraPath("Cam", "0x11000007ca313a", &status);
  auto info = cs::GetUsbCameraInfo(source, &status);
  std::cout << info.name << " " << info.path << std::endl;

  auto res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, false);
  cs::SetUsbCameraPath(source, "EAB7A68F-EC2B-4487-AADF-D8A91C1CB782", &status);

  //auto runLoop = CFRunLoopGetMain();


  res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 20, false);
  printf("%d\n", res);
  //std::this_thread::sleep_for(std::chrono::seconds(20));

  cs::ReleaseSource(source, &status);


}
