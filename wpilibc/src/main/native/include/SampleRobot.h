/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "RobotBase.h"

namespace frc {

class SampleRobot : public RobotBase {
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
