// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include "wpi/cs/MjpegServer.hpp"
#include "wpi/cs/RawEvent.hpp"
#include "wpi/cs/UsbCamera.hpp"
#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/util/print.hpp"

int main() {
  wpi::util::print("hostname: {}\n", wpi::cs::GetHostname());
  std::puts("IPv4 network addresses:");
  for (const auto& addr : wpi::cs::GetNetworkInterfaces()) {
    wpi::util::print("  {}\n", addr);
  }
  wpi::cs::UsbCamera camera{"usbcam", 0};
  camera.SetVideoMode(wpi::util::PixelFormat::kMJPEG, 320, 240, 30);
  wpi::cs::MjpegServer mjpegServer{"httpserver", 8081};
  mjpegServer.SetSource(camera);

  CS_Status status = 0;
  wpi::cs::AddListener(
      [&](const wpi::cs::RawEvent& event) {
        wpi::util::print("FPS={} MBPS={}\n", camera.GetActualFPS(),
                         (camera.GetActualDataRate() / 1000000.0));
      },
      wpi::cs::RawEvent::kTelemetryUpdated, false, &status);
  wpi::cs::SetTelemetryPeriod(1.0);

  std::getchar();
}
