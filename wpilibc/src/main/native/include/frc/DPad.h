/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/Joystick.h>
#include <frc/buttons/POVButton.h>

namespace frc {
class DPad {
 public:
  DPad(GenericHID& joystick, int povNumber = 0);
  virtual ~DPad() = default;

  DPad(DPad&&) = default;
  DPad& operator=(DPad&&) = default;

  frc::POVButton up;
  frc::POVButton right;
  frc::POVButton down;
  frc::POVButton left;

 private:
  enum class Angles {
    kUp = 0,
    kRight = 90,
    kDown = 180,
    kLeft = 270,
  };
};
}  // namespace frc
