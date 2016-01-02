/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __WAIT_FOR_CHILDREN_H__
#define __WAIT_FOR_CHILDREN_H__

#include "Commands/Command.h"

class WaitForChildren : public Command {
 public:
  WaitForChildren(double timeout);
  WaitForChildren(const std::string &name, double timeout);
  virtual ~WaitForChildren() = default;

 protected:
  virtual void Initialize();
  virtual void Execute();
  virtual bool IsFinished();
  virtual void End();
  virtual void Interrupted();
};

#endif
