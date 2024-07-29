// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <wpi/print.h>

#include "cscore.h"

int main() {
  CS_Status status = 0;

  for (const auto& caminfo : cs::EnumerateUsbCameras(&status)) {
    wpi::print("{}: {} ({})\n", caminfo.dev, caminfo.path, caminfo.name);
    if (!caminfo.otherPaths.empty()) {
      std::puts("Other device paths:");
      for (auto&& path : caminfo.otherPaths) {
        wpi::print("  {}\n", path);
      }
    }

    cs::UsbCamera camera{"usbcam", caminfo.dev};

    std::puts("Properties:");
    for (const auto& prop : camera.EnumerateProperties()) {
      wpi::print("  {}", prop.GetName());
      switch (prop.GetKind()) {
        case cs::VideoProperty::kBoolean:
          wpi::print(" (bool): value={} default={}", prop.Get(),
                     prop.GetDefault());
          break;
        case cs::VideoProperty::kInteger:
          wpi::print(" (int): value={} min={} max={} step={} default={}",
                     prop.Get(), prop.GetMin(), prop.GetMax(), prop.GetStep(),
                     prop.GetDefault());
          break;
        case cs::VideoProperty::kString:
          wpi::print(" (string): {}", prop.GetString());
          break;
        case cs::VideoProperty::kEnum: {
          wpi::print(" (enum): value={}", prop.Get());
          auto choices = prop.GetChoices();
          for (size_t i = 0; i < choices.size(); ++i) {
            if (!choices[i].empty()) {
              wpi::print("\n    {}: {}", i, choices[i]);
            }
          }
          break;
        }
        default:
          break;
      }
      std::fputc('\n', stdout);
    }

    std::puts("Video Modes:");
    for (const auto& mode : camera.EnumerateVideoModes()) {
      const char* pixelFormat;
      switch (mode.pixelFormat) {
        case cs::VideoMode::kMJPEG:
          pixelFormat = "MJPEG";
          break;
        case cs::VideoMode::kYUYV:
          pixelFormat = "YUYV";
          break;
        case cs::VideoMode::kRGB565:
          pixelFormat = "RGB565";
          break;
        default:
          pixelFormat = "Unknown";
          break;
      }
      wpi::print("  PixelFormat:{} Width:{} Height:{} FPS:{}\n", pixelFormat,
                 mode.width, mode.height, mode.fps);
    }
  }
}
