/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DPad.h"

using namespace frc;
DPad::DPad(GenericHID& joystick, int povNumber)
    : up(joystick, static_cast<int>(Angles::kUp), povNumber),
      right(joystick, static_cast<int>(Angles::kRight), povNumber),
      down(joystick, static_cast<int>(Angles::kDown), povNumber),
      left(joystick, static_cast<int>(Angles::kLeft), povNumber) {}
