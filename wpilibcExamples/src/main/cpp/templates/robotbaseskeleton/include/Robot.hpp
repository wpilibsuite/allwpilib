// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>

#include <frc/RobotBase.h>

class Robot : public frc::RobotBase {
 public:
  Robot();
  void Disabled();
  void Autonomous();
  void Teleop();
  void Test();

  void StartCompetition() override;
  void EndCompetition() override;

 private:
  std::atomic<bool> m_exit{false};
};
