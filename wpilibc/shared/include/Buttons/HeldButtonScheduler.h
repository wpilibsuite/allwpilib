/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __HELD_BUTTON_SCHEDULER_H__
#define __HELD_BUTTON_SCHEDULER_H__

#include "Buttons/ButtonScheduler.h"

class Trigger;
class Command;

class HeldButtonScheduler : public ButtonScheduler {
 public:
  HeldButtonScheduler(bool last, Trigger *button, Command *orders);
  virtual ~HeldButtonScheduler() = default;
  virtual void Execute();
};

#endif
