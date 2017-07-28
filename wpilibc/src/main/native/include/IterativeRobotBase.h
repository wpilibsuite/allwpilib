/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
 * Init() functions -- each of the following functions is called once when the
 *                     appropriate mode is entered:
 *   - DisabledInit()   -- called only when first disabled
 *   - AutonomousInit() -- called each and every time autonomous is entered from
 *                         another mode
 *   - TeleopInit()     -- called each and every time teleop is entered from
 *                         another mode
 *   - TestInit()       -- called each and every time test is entered from
 *                         another mode
 *
 * Periodic() functions -- each of these functions is called on an interval:
 *   - RobotPeriodic()
 *   - DisabledPeriodic()
 *   - AutonomousPeriodic()
 *   - TeleopPeriodic()
 *   - TestPeriodic()
 */
class IterativeRobotBase : public RobotBase {
 public:
  virtual void RobotInit();
  virtual void DisabledInit();
  virtual void AutonomousInit();
  virtual void TeleopInit();
  virtual void TestInit();

  virtual void RobotPeriodic();
  virtual void DisabledPeriodic();
  virtual void AutonomousPeriodic();
  virtual void TeleopPeriodic();
  virtual void TestPeriodic();

 protected:
  IterativeRobotBase();
  virtual ~IterativeRobotBase() = default;

  void LoopFunc();

 private:
  enum class Mode { kNone, kDisabled, kAutonomous, kTeleop, kTest };

  Mode m_lastMode = Mode::kNone;
};

}  // namespace frc
