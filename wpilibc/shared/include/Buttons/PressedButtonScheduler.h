/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __PRESSED_BUTTON_SCHEDULER_H__
#define __PRESSED_BUTTON_SCHEDULER_H__

#include "Buttons/ButtonScheduler.h"

class Trigger;
class Command;

class PressedButtonScheduler : public ButtonScheduler {
 public:
  PressedButtonScheduler(bool last, Trigger *button, Command *orders);
  virtual ~PressedButtonScheduler() = default;
  virtual void Execute();
};

#endif
