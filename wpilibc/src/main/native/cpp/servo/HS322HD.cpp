// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/servo/HS322HD.h"

using namespace frc;

HS322HD::HS322HD(int channel)
    : RotaryServo("HS322HD", channel, 0.0_rad, 180.0_rad) {
  SetBounds(2.4, 0, 0, 0, 0.6);
}
