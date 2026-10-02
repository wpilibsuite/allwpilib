// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi::glass {
class EncodersModel;
}  // namespace wpi::glass

namespace halsimgui {

class EncoderSimGui {
 public:
  static void Initialize();
  static wpi::glass::EncodersModel& GetEncodersModel();
};

}  // namespace halsimgui
