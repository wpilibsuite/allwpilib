// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
                           wpi::span<Subsystem* const> requirements) {
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
                         wpi::span<Subsystem* const> requirements) {
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
                            wpi::span<Subsystem* const> requirements) {
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
