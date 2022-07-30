// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command/button/Button.h"

using namespace frc;

Button::Button(std::function<bool()> isPressed) : Trigger(isPressed) {}

Button Button::WhenPressed(Command* command) {
  WhenActive(command);
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

Button Button::WhileHeld(Command* command) {
  WhileActiveContinous(command);
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

Button Button::WhenHeld(Command* command) {
  WhileActiveOnce(command);
  return *this;
}

Button Button::WhenReleased(Command* command) {
  WhenInactive(command);
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

Button Button::ToggleWhenPressed(Command* command) {
  ToggleWhenActive(command);
  return *this;
}

Button Button::CancelWhenPressed(Command* command) {
  CancelWhenActive(command);
  return *this;
}
