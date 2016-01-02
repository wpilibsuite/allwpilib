/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __WAIT_UNTIL_COMMAND_H__
#define __WAIT_UNTIL_COMMAND_H__

#include "Commands/Command.h"

class WaitUntilCommand : public Command {
 public:
  WaitUntilCommand(double time);
  WaitUntilCommand(const std::string &name, double time);
  virtual ~WaitUntilCommand() = default;

 protected:
  virtual void Initialize();
  virtual void Execute();
  virtual bool IsFinished();
  virtual void End();
  virtual void Interrupted();

 private:
  double m_time;
};

#endif
