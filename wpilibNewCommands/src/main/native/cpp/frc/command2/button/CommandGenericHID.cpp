// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/button/CommandGenericHID.h"

using namespace frc;

Trigger CommandGenericHID::Button(int button, EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandGenericHID::POV(int angle, EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, int angle, EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return this->GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(EventLoop* loop) const {
  return POV(0, loop);
}

Trigger CommandGenericHID::POVUpRight(EventLoop* loop) const {
  return POV(45, loop);
}

Trigger CommandGenericHID::POVRight(EventLoop* loop) const {
  return POV(90, loop);
}

Trigger CommandGenericHID::POVDownRight(EventLoop* loop) const {
  return POV(135, loop);
}

Trigger CommandGenericHID::POVDown(EventLoop* loop) const {
  return POV(180, loop);
}

Trigger CommandGenericHID::POVDownLeft(EventLoop* loop) const {
  return POV(225, loop);
}

Trigger CommandGenericHID::POVLeft(EventLoop* loop) const {
  return POV(270, loop);
}

Trigger CommandGenericHID::POVUpLeft(EventLoop* loop) const {
  return POV(315, loop);
}

Trigger CommandGenericHID::POVCenter(EventLoop* loop) const {
  return POV(360, loop);
}

Trigger CommandGenericHID::AxisLessThan(int axis, double threshold,
                                        EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return this->GetRawAxis(axis) < threshold;
  });
}

Trigger CommandGenericHID::AxisGreaterThan(int axis, double threshold,
                                           EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return this->GetRawAxis(axis) > threshold;
  });
}
