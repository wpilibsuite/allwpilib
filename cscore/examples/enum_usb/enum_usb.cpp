// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include "wpi/cs/cscore.h"
#include "wpi/util/print.hpp"

int main() {
  CS_Status status = 0;

  for (const auto& caminfo : wpi::cs::EnumerateUsbCameras(&status)) {
    wpi::util::print("{}: {} ({})\n", caminfo.dev, caminfo.path, caminfo.name);
    if (!caminfo.otherPaths.empty()) {
      std::puts("Other device paths:");
      for (auto&& path : caminfo.otherPaths) {
        wpi::util::print("  {}\n", path);
      }
    }

    wpi::cs::UsbCamera camera{"usbcam", caminfo.dev};

    std::puts("Properties:");
    for (const auto& prop : camera.EnumerateProperties()) {
      wpi::util::print("  {}", prop.GetName());
      switch (prop.GetKind()) {
        case wpi::cs::VideoProperty::kBoolean:
          wpi::util::print(" (bool): value={} default={}", prop.Get(),
                     prop.GetDefault());
          break;
        case wpi::cs::VideoProperty::kInteger:
          wpi::util::print(" (int): value={} min={} max={} step={} default={}",
                     prop.Get(), prop.GetMin(), prop.GetMax(), prop.GetStep(),
                     prop.GetDefault());
          break;
        case wpi::cs::VideoProperty::kString:
          wpi::util::print(" (string): {}", prop.GetString());
          break;
        case wpi::cs::VideoProperty::kEnum: {
          wpi::util::print(" (enum): value={}", prop.Get());
          auto choices = prop.GetChoices();
          for (size_t i = 0; i < choices.size(); ++i) {
            if (!choices[i].empty()) {
              wpi::util::print("\n    {}: {}", i, choices[i]);
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
        case wpi::cs::VideoMode::kMJPEG:
          pixelFormat = "MJPEG";
          break;
        case wpi::cs::VideoMode::kYUYV:
          pixelFormat = "YUYV";
          break;
        case wpi::cs::VideoMode::kRGB565:
          pixelFormat = "RGB565";
          break;
        default:
          pixelFormat = "Unknown";
          break;
      }
      wpi::util::print("  PixelFormat:{} Width:{} Height:{} FPS:{}\n", pixelFormat,
                 mode.width, mode.height, mode.fps);
    }
  }
}
