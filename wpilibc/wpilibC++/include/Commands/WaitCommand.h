/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __WAIT_COMMAND_H__
#define __WAIT_COMMAND_H__

#include "Commands/Command.h"

class WaitCommand : public Command {
 public:
  WaitCommand(double timeout);
  WaitCommand(const std::string &name, double timeout);
  virtual ~WaitCommand() = default;

 protected:
  virtual void Initialize();
  virtual void Execute();
  virtual bool IsFinished();
  virtual void End();
  virtual void Interrupted();
};

#endif
