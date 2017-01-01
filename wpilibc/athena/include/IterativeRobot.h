/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "RobotBase.h"

namespace frc {

/**
 * IterativeRobot implements a specific type of Robot Program framework,
 * extending the RobotBase class.
 *
 * The IterativeRobot class is intended to be subclassed by a user creating a
 * robot program.
 *
 * This class is intended to implement the "old style" default code, by
 * providing the following functions which are called by the main loop,
 * StartCompetition(), at the appropriate times:
 *
 * RobotInit() -- provide for initialization at robot power-on
 *
 * Init() functions -- each of the following functions is called once when the
 *                     appropriate mode is entered:
 *  - DisabledInit()   -- called only when first disabled
 *  - AutonomousInit() -- called each and every time autonomous is entered from
 *                        another mode
 *  - TeleopInit()     -- called each and every time teleop is entered from
 *                        another mode
 *  - TestInit()       -- called each and every time test is entered from
 *                        another mode
 *
 * Periodic() functions -- each of these functions is called each time a
 *                         new packet is received from the driver station:
 *   - RobotPeriodic()
 *   - DisabledPeriodic()
 *   - AutonomousPeriodic()
 *   - TeleopPeriodic()
 *   - TestPeriodic()
 *
 */

class IterativeRobot : public RobotBase {
 public:
  virtual void StartCompetition();

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
  virtual ~IterativeRobot() = default;
  IterativeRobot() = default;

 private:
  bool m_disabledInitialized = false;
  bool m_autonomousInitialized = false;
  bool m_teleopInitialized = false;
  bool m_testInitialized = false;
};

}  // namespace frc
