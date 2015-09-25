/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __TOGGLE_BUTTON_SCHEDULER_H__
#define __TOGGLE_BUTTON_SCHEDULER_H__

#include "Buttons/ButtonScheduler.h"

class Trigger;
class Command;

class ToggleButtonScheduler : public ButtonScheduler {
 public:
  ToggleButtonScheduler(bool last, Trigger *button, Command *orders);
  virtual ~ToggleButtonScheduler() = default;
  virtual void Execute();

 private:
  bool pressedLast;
};

#endif
