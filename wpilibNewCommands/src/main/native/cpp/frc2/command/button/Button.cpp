/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/button/Button.h"

using namespace frc2;

Button::Button(std::function<bool()> isPressed) : Trigger(isPressed) {}

Button Button::WhenPressed(Command* command, bool interruptible) {
  WhenActive(command, interruptible);
  return *this;
}

Button Button::WhenPressed(std::function<void()> toRun,
                           std::initializer_list<Subsystem*> requirements) {
  WhenActive(std::move(toRun), requirements);
  return *this;
}

Button Button::WhenPressed(std::function<void()> toRun,
                           wpi::ArrayRef<Subsystem*> requirements) {
  WhenActive(std::move(toRun), requirements);
  return *this;
}

Button Button::WhileHeld(Command* command, bool interruptible) {
  WhileActiveContinous(command, interruptible);
  return *this;
}

Button Button::WhileHeld(std::function<void()> toRun,
                         std::initializer_list<Subsystem*> requirements) {
  WhileActiveContinous(std::move(toRun), requirements);
  return *this;
}

Button Button::WhileHeld(std::function<void()> toRun,
                         wpi::ArrayRef<Subsystem*> requirements) {
  WhileActiveContinous(std::move(toRun), requirements);
  return *this;
}

Button Button::WhenHeld(Command* command, bool interruptible) {
  WhileActiveOnce(command, interruptible);
  return *this;
}

Button Button::WhenReleased(Command* command, bool interruptible) {
  WhenInactive(command, interruptible);
  return *this;
}

Button Button::WhenReleased(std::function<void()> toRun,
                            std::initializer_list<Subsystem*> requirements) {
  WhenInactive(std::move(toRun), requirements);
  return *this;
}

Button Button::WhenReleased(std::function<void()> toRun,
                            wpi::ArrayRef<Subsystem*> requirements) {
  WhenInactive(std::move(toRun), requirements);
  return *this;
}

Button Button::ToggleWhenPressed(Command* command, bool interruptible) {
  ToggleWhenActive(command, interruptible);
  return *this;
}

Button Button::CancelWhenPressed(Command* command) {
  CancelWhenActive(command);
  return *this;
}
