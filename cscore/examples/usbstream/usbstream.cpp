/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdio>

#include <wpi/raw_ostream.h>

#include "cscore.h"

int main() {
  wpi::outs() << "hostname: " << cs::GetHostname() << '\n';
  wpi::outs() << "IPv4 network addresses:\n";
  for (const auto& addr : cs::GetNetworkInterfaces())
    wpi::outs() << "  " << addr << '\n';
  cs::UsbCamera camera{"usbcam", 0};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::MjpegServer mjpegServer{"httpserver", 8081};
  mjpegServer.SetSource(camera);

  CS_Status status = 0;
  cs::AddListener(
      [&](const cs::RawEvent& event) {
        wpi::outs() << "FPS=" << camera.GetActualFPS()
                    << " MBPS=" << (camera.GetActualDataRate() / 1000000.0)
                    << '\n';
      },
      cs::RawEvent::kTelemetryUpdated, false, &status);
  cs::SetTelemetryPeriod(1.0);

  std::getchar();
}
