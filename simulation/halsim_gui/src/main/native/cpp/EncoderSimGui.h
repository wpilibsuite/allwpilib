/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
