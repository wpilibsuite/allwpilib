// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <wpi/print.h>

#include "cscore.h"

int main() {
  wpi::print("hostname: {}\n", cs::GetHostname());
  std::puts("IPv4 network addresses:");
  for (const auto& addr : cs::GetNetworkInterfaces()) {
    wpi::print("  {}\n", addr);
  }
  cs::UsbCamera camera{"usbcam", 0};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::MjpegServer mjpegServer{"httpserver", 8081};
  mjpegServer.SetSource(camera);

  CS_Status status = 0;
  cs::AddListener(
      [&](const cs::RawEvent& event) {
        wpi::print("FPS={} MBPS={}\n", camera.GetActualFPS(),
                   (camera.GetActualDataRate() / 1000000.0));
      },
      cs::RawEvent::kTelemetryUpdated, false, &status);
  cs::SetTelemetryPeriod(1.0);

  std::getchar();
}
