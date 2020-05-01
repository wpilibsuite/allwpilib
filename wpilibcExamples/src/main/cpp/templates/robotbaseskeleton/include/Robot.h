/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

#include <frc/RobotBase.h>

class Robot : public frc::RobotBase {
 public:
  void RobotInit();
  void Disabled();
  void Autonomous();
  void Teleop();
  void Test();

  void StartCompetition() override;
  void EndCompetition() override;

 private:
  std::atomic<bool> m_exit{false};
};
