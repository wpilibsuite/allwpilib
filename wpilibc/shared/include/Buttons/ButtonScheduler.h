/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
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
