/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "IterativeBase.h"
#include "RobotBase.h"

namespace frc {

/**
 * IterativeRobotBase implements a specific type of robot program framework,
 * extending the RobotBase class.
 *
 * The IterativeRobotBase class does not implement StartCompetition(), so it
 * should not be used by teams directly.
 *
 * This class provides the following functions which are called by the main
 * loop, StartCompetition(), at the appropriate times:
 *
 * RobotInit() -- provide for initialization at robot power-on
 *
 * Init() and Periodic() functions are implemented from, and documented within,
 * IterativeBase.
 */
class IterativeRobotBase : public RobotBase, public IterativeBase {
 public:
  void RobotInit() override;
  void DisabledInit() override;
  void AutonomousInit() override;
  void TeleopInit() override;
  void TestInit() override;

  void RobotPeriodic() override;
  void DisabledPeriodic() override;
  void AutonomousPeriodic() override;
  void TeleopPeriodic() override;
  void TestPeriodic() override;

 protected:
  virtual ~IterativeRobotBase() = default;

  void LoopFunc();

 private:
  enum class Mode { kNone, kDisabled, kAutonomous, kTeleop, kTest };

  Mode m_lastMode = Mode::kNone;
};

}  // namespace frc
