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
//0x111000007ca313a
//EAB7A68F-EC2B-4487-AADF-D8A91C1CB782

  //CS_Source source = cs::CreateUsbCameraPath("Cam", "0x10000007ca313a", &status);
  CS_Source source2 = cs::CreateUsbCameraPath("Cam2", "EAB7A68F-EC2B-4487-AADF-D8A91C1CB782", &status);

  //cs::VideoMode mode{cs::VideoMode::PixelFormat::kBGR, 640, 480, 5};
  //cs::SetSourceVideoMode(source2, mode, &status);

  //cs::SetSourceFPS(source2, 50, &status);
  printf("Status %d\n", status);

  //auto info = cs::GetUsbCameraInfo(source, &status);
  //std::cout << info.name << " " << info.path << std::endl;

  auto res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, false);
  cs::SetSourceConnectionStrategy(source2, CS_CONNECTION_KEEP_OPEN, &status);
  auto modes = cs::EnumerateSourceVideoModes(source2, &status);
  std::cout << modes.size() << std::endl;
  for (auto&& mode : modes) {
    std::cout << mode.width << "x" << mode.height << " at " << mode.fps << " FPS " << mode.pixelFormat << std::endl;
  }

  //CS_Sink sink = cs::CreateMjpegServer("Server", "", 1234, &status);
  //cs::SetSinkSource(sink, source, &status);

  CS_Sink sink2 = cs::CreateMjpegServer("Server2", "", 1235, &status);
  cs::SetSinkSource(sink2, source2, &status);


  //cs::SetUsbCameraPath(source, "EAB7A68F-EC2B-4487-AADF-D8A91C1CB782", &status);

  //auto runLoop = CFRunLoopGetMain();


  res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 60, false);
  printf("%d\n", res);
  //std::this_thread::sleep_for(std::chrono::seconds(20));

  //cs::ReleaseSource(source, &status);


}
