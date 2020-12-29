// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

class Trigger;
class Command;

class ButtonScheduler {
 public:
  ButtonScheduler(bool last, Trigger* button, Command* orders);
  virtual ~ButtonScheduler() = default;

  ButtonScheduler(ButtonScheduler&&) = default;
  ButtonScheduler& operator=(ButtonScheduler&&) = default;

  virtual void Execute() = 0;
  void Start();

 protected:
  bool m_pressedLast;
  Trigger* m_button;
  Command* m_command;
};

}  // namespace frc
