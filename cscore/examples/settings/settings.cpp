// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_ostream.h>

#include "cscore.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    wpi::errs() << "Usage: settings camera [prop val] ... -- [prop val]...\n";
    wpi::errs() << "  Example: settings 1 brightness 30 raw_contrast 10\n";
    return 1;
  }

  int id;
  if (auto v = wpi::parse_integer<int>(argv[1], 10)) {
    id = v.value();
  } else {
    wpi::errs() << "Expected number for camera\n";
    return 2;
  }

  cs::UsbCamera camera{"usbcam", id};

  // Set prior to connect
  int arg = 2;
  std::string_view propName;
  for (; arg < argc && std::string_view{argv[arg]} != "--"; ++arg) {
    if (propName.empty()) {
      propName = argv[arg];
    } else {
      std::string_view propVal{argv[arg]};
      if (auto v = wpi::parse_integer<int>(propVal, 10)) {
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
      if (auto v = wpi::parse_integer<int>(propVal, 10)) {
        camera.GetProperty(propName).Set(v.value());
      } else {
        camera.GetProperty(propName).SetString(propVal);
      }
      propName = {};
    }
  }

  // Print settings
  wpi::SmallString<64> buf;
  wpi::outs() << "Properties:\n";
  for (const auto& prop : camera.EnumerateProperties()) {
    wpi::outs() << "  " << prop.GetName();
    switch (prop.GetKind()) {
      case cs::VideoProperty::kBoolean:
        wpi::outs() << " (bool): "
                    << "value=" << prop.Get()
                    << " default=" << prop.GetDefault();
        break;
      case cs::VideoProperty::kInteger:
        wpi::outs() << " (int): "
                    << "value=" << prop.Get() << " min=" << prop.GetMin()
                    << " max=" << prop.GetMax() << " step=" << prop.GetStep()
                    << " default=" << prop.GetDefault();
        break;
      case cs::VideoProperty::kString:
        wpi::outs() << " (string): " << prop.GetString(buf);
        break;
      case cs::VideoProperty::kEnum: {
        wpi::outs() << " (enum): "
                    << "value=" << prop.Get();
        auto choices = prop.GetChoices();
        for (size_t i = 0; i < choices.size(); ++i) {
          if (choices[i].empty()) {
            continue;
          }
          wpi::outs() << "\n    " << i << ": " << choices[i];
        }
        break;
      }
      default:
        break;
    }
    wpi::outs() << '\n';
  }
}
