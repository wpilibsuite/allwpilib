// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace glass {
class EncodersModel;
}  // namespace glass

namespace halsimgui {

class EncoderSimGui {
 public:
  static void Initialize();
  static glass::EncodersModel& GetEncodersModel();
};

}  // namespace halsimgui
