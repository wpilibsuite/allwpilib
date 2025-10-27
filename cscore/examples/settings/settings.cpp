// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdio>
#include <thread>

#include <wpi/util/StringExtras.hpp>
#include <wpi/util/print.hpp>

#include "wpi/cs/cscore.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::fputs("Usage: settings camera [prop val] ... -- [prop val]...\n",
               stderr);
    std::fputs("  Example: settings 1 brightness 30 raw_contrast 10\n", stderr);
    return 1;
  }

  int id;
  if (auto v = wpi::util::parse_integer<int>(argv[1], 10)) {
    id = v.value();
  } else {
    std::fputs("Expected number for camera\n", stderr);
    return 2;
  }

  wpi::cs::UsbCamera camera{"usbcam", id};

  // Set prior to connect
  int arg = 2;
  std::string_view propName;
  for (; arg < argc && std::string_view{argv[arg]} != "--"; ++arg) {
    if (propName.empty()) {
      propName = argv[arg];
    } else {
      std::string_view propVal{argv[arg]};
      if (auto v = wpi::util::parse_integer<int>(propVal, 10)) {
        camera.GetProperty(propName).Set(v.value());
      } else {
        camera.GetProperty(propName).SetString(propVal);
      }
      propName = {};
    }
  }
  if (arg < argc && std::string_view{argv[arg]} == "--") {
    ++arg;
  }

  // Wait to connect
  while (!camera.IsConnected()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Set rest
  propName = {};
  for (; arg < argc; ++arg) {
    if (propName.empty()) {
      propName = argv[arg];
    } else {
      std::string_view propVal{argv[arg]};
      if (auto v = wpi::util::parse_integer<int>(propVal, 10)) {
        camera.GetProperty(propName).Set(v.value());
      } else {
        camera.GetProperty(propName).SetString(propVal);
      }
      propName = {};
    }
  }

  // Print settings
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
}
