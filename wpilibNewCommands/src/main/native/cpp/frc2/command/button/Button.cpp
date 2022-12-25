// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/Button.h"

using namespace frc2;

Button::Button(std::function<bool()> isPressed) : Trigger(isPressed) {}

Button Button::WhenPressed(Command* command) {
  WPI_IGNORE_DEPRECATED
  WhenActive(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenPressed(std::function<void()> toRun,
                           std::initializer_list<Subsystem*> requirements) {
  WPI_IGNORE_DEPRECATED
  WhenActive(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenPressed(std::function<void()> toRun,
                           std::span<Subsystem* const> requirements) {
  WPI_IGNORE_DEPRECATED
  WhenActive(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhileHeld(Command* command) {
  WPI_IGNORE_DEPRECATED
  WhileActiveContinous(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhileHeld(std::function<void()> toRun,
                         std::initializer_list<Subsystem*> requirements) {
  WPI_IGNORE_DEPRECATED
  WhileActiveContinous(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhileHeld(std::function<void()> toRun,
                         std::span<Subsystem* const> requirements) {
  WPI_IGNORE_DEPRECATED
  WhileActiveContinous(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenHeld(Command* command) {
  WPI_IGNORE_DEPRECATED
  WhileActiveOnce(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenReleased(Command* command) {
  WPI_IGNORE_DEPRECATED
  WhenInactive(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenReleased(std::function<void()> toRun,
                            std::initializer_list<Subsystem*> requirements) {
  WPI_IGNORE_DEPRECATED
  WhenInactive(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::WhenReleased(std::function<void()> toRun,
                            std::span<Subsystem* const> requirements) {
  WPI_IGNORE_DEPRECATED
  WhenInactive(std::move(toRun), requirements);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::ToggleWhenPressed(Command* command) {
  WPI_IGNORE_DEPRECATED
  ToggleWhenActive(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}

Button Button::CancelWhenPressed(Command* command) {
  WPI_IGNORE_DEPRECATED
  CancelWhenActive(command);
  WPI_UNIGNORE_DEPRECATED
  return *this;
}
