// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandGenericHID.h"

using namespace frc2;

Trigger CommandGenericHID::Button(int button, frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandGenericHID::POV(int angle, frc::EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, int angle, frc::EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return this->GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(frc::EventLoop* loop) const {
  return POV(0, loop);
}

Trigger CommandGenericHID::POVUpRight(frc::EventLoop* loop) const {
  return POV(45, loop);
}

Trigger CommandGenericHID::POVRight(frc::EventLoop* loop) const {
  return POV(90, loop);
}

Trigger CommandGenericHID::POVDownRight(frc::EventLoop* loop) const {
  return POV(135, loop);
}

Trigger CommandGenericHID::POVDown(frc::EventLoop* loop) const {
  return POV(180, loop);
}

Trigger CommandGenericHID::POVDownLeft(frc::EventLoop* loop) const {
  return POV(225, loop);
}

Trigger CommandGenericHID::POVLeft(frc::EventLoop* loop) const {
  return POV(270, loop);
}

Trigger CommandGenericHID::POVUpLeft(frc::EventLoop* loop) const {
  return POV(315, loop);
}

Trigger CommandGenericHID::POVCenter(frc::EventLoop* loop) const {
  return POV(360, loop);
}
