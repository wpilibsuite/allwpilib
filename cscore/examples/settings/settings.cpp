/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <thread>

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "cscore.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    wpi::errs() << "Usage: settings camera [prop val] ... -- [prop val]...\n";
    wpi::errs() << "  Example: settings 1 brightness 30 raw_contrast 10\n";
    return 1;
  }

  int id;
  if (wpi::StringRef{argv[1]}.getAsInteger(10, id)) {
    wpi::errs() << "Expected number for camera\n";
    return 2;
  }

  cs::UsbCamera camera{"usbcam", id};

  // Set prior to connect
  int arg = 2;
  wpi::StringRef propName;
  for (; arg < argc && wpi::StringRef{argv[arg]} != "--"; ++arg) {
    if (propName.empty()) {
      propName = argv[arg];
    } else {
      wpi::StringRef propVal{argv[arg]};
      int intVal;
      if (propVal.getAsInteger(10, intVal))
        camera.GetProperty(propName).SetString(propVal);
      else
        camera.GetProperty(propName).Set(intVal);
      propName = wpi::StringRef{};
    }
  }
  if (arg < argc && wpi::StringRef{argv[arg]} == "--") ++arg;

  // Wait to connect
  while (!camera.IsConnected())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Set rest
  propName = wpi::StringRef{};
  for (; arg < argc; ++arg) {
    if (propName.empty()) {
      propName = argv[arg];
    } else {
      wpi::StringRef propVal{argv[arg]};
      int intVal;
      if (propVal.getAsInteger(10, intVal))
        camera.GetProperty(propName).SetString(propVal);
      else
        camera.GetProperty(propName).Set(intVal);
      propName = wpi::StringRef{};
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
          if (choices[i].empty()) continue;
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
