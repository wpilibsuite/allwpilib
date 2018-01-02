/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <llvm/SmallString.h>
#include <llvm/raw_ostream.h>

#include "cscore.h"

int main() {
  CS_Status status = 0;
  llvm::SmallString<64> buf;

  for (const auto& caminfo : cs::EnumerateUsbCameras(&status)) {
    llvm::outs() << caminfo.dev << ": " << caminfo.path << " (" << caminfo.name
                 << ")\n";
    cs::UsbCamera camera{"usbcam", caminfo.dev};

    llvm::outs() << "Properties:\n";
    for (const auto& prop : camera.EnumerateProperties()) {
      llvm::outs() << "  " << prop.GetName();
      switch (prop.GetKind()) {
        case cs::VideoProperty::kBoolean:
          llvm::outs() << " (bool): "
                       << "value=" << prop.Get()
                       << " default=" << prop.GetDefault();
          break;
        case cs::VideoProperty::kInteger:
          llvm::outs() << " (int): "
                       << "value=" << prop.Get() << " min=" << prop.GetMin()
                       << " max=" << prop.GetMax() << " step=" << prop.GetStep()
                       << " default=" << prop.GetDefault();
          break;
        case cs::VideoProperty::kString:
          llvm::outs() << " (string): " << prop.GetString(buf);
          break;
        case cs::VideoProperty::kEnum: {
          llvm::outs() << " (enum): "
                       << "value=" << prop.Get();
          auto choices = prop.GetChoices();
          for (size_t i = 0; i < choices.size(); ++i) {
            if (choices[i].empty()) continue;
            llvm::outs() << "\n    " << i << ": " << choices[i];
          }
          break;
        }
        default:
          break;
      }
      llvm::outs() << '\n';
    }

    llvm::outs() << "Video Modes:\n";
    for (const auto& mode : camera.EnumerateVideoModes()) {
      llvm::outs() << "  PixelFormat:";
      switch (mode.pixelFormat) {
        case cs::VideoMode::kMJPEG:
          llvm::outs() << "MJPEG";
          break;
        case cs::VideoMode::kYUYV:
          llvm::outs() << "YUYV";
          break;
        case cs::VideoMode::kRGB565:
          llvm::outs() << "RGB565";
          break;
        default:
          llvm::outs() << "Unknown";
          break;
      }
      llvm::outs() << " Width:" << mode.width;
      llvm::outs() << " Height:" << mode.height;
      llvm::outs() << " FPS:" << mode.fps << '\n';
    }
  }
}
