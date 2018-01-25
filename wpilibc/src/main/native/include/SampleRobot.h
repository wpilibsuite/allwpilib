/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <support/deprecated.h>

#include "RobotBase.h"

namespace frc {

class WPI_DEPRECATED(
    "WARNING: While it may look like a good choice to use for your code if "
    "you're inexperienced, don't. Unless you know what you are doing, complex "
    "code will be much more difficult under this system. Use TimedRobot or "
    "Command-Based instead.") SampleRobot : public RobotBase {
 public:
  void StartCompetition() override;

  virtual void RobotInit();
  virtual void Disabled();
  virtual void Autonomous();
  virtual void OperatorControl();
  virtual void Test();
  virtual void RobotMain();

 protected:
  SampleRobot();
  virtual ~SampleRobot() = default;

 private:
  bool m_robotMainOverridden = true;
};

}  // namespace frc
