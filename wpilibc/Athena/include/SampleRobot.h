/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "RobotBase.h"

class SampleRobot : public RobotBase {
 public:
  SampleRobot();
  virtual ~SampleRobot() = default;
  virtual void RobotInit();
  virtual void Disabled();
  virtual void Autonomous();
  virtual void OperatorControl();
  virtual void Test();
  virtual void RobotMain();
  void StartCompetition();

 private:
  bool m_robotMainOverridden;
};
