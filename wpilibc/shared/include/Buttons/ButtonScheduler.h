/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __BUTTON_SCHEDULER_H__
#define __BUTTON_SCHEDULER_H__

class Trigger;
class Command;

class ButtonScheduler {
 public:
  ButtonScheduler(bool last, Trigger *button, Command *orders);
  virtual ~ButtonScheduler() = default;
  virtual void Execute() = 0;
  void Start();

 protected:
  bool m_pressedLast;
  Trigger *m_button;
  Command *m_command;
};

#endif
