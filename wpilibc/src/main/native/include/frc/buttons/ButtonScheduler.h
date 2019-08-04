/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
