/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "cscore.h"

int main() {
  CS_Status status = 0;
  wpi::SmallString<64> buf;

  for (const auto& caminfo : cs::EnumerateUsbCameras(&status)) {
    wpi::outs() << caminfo.dev << ": " << caminfo.path << " (" << caminfo.name
                << ")\n";
    if (!caminfo.otherPaths.empty()) {
      wpi::outs() << "Other device paths:\n";
      for (auto&& path : caminfo.otherPaths)
        wpi::outs() << "  " << path << '\n';
    }

    cs::UsbCamera camera{"usbcam", caminfo.dev};

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

    wpi::outs() << "Video Modes:\n";
    for (const auto& mode : camera.EnumerateVideoModes()) {
      wpi::outs() << "  PixelFormat:";
      switch (mode.pixelFormat) {
        case cs::VideoMode::kMJPEG:
          wpi::outs() << "MJPEG";
          break;
        case cs::VideoMode::kYUYV:
          wpi::outs() << "YUYV";
          break;
        case cs::VideoMode::kRGB565:
          wpi::outs() << "RGB565";
          break;
        default:
          wpi::outs() << "Unknown";
          break;
      }
      wpi::outs() << " Width:" << mode.width;
      wpi::outs() << " Height:" << mode.height;
      wpi::outs() << " FPS:" << mode.fps << '\n';
    }
  }
}
