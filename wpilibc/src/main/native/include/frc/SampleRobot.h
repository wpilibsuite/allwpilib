/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/deprecated.h>

#include "frc/RobotBase.h"

namespace frc {

class SampleRobot : public RobotBase {
 public:
  /**
   * Start a competition.
   *
   * This code needs to track the order of the field starting to ensure that
   * everything happens in the right order. Repeatedly run the correct method,
   * either Autonomous or OperatorControl or Test when the robot is enabled.
   * After running the correct method, wait for some state to change, either the
   * other mode starts or the robot is disabled. Then go back and wait for the
   * robot to be enabled again.
   */
  void StartCompetition() override;

  /**
   * Robot-wide initialization code should go here.
   *
   * Users should override this method for default Robot-wide initialization
   * which will be called when the robot is first powered on. It will be called
   * exactly one time.
   *
   * Warning: the Driver Station "Robot Code" light and FMS "Robot Ready"
   * indicators will be off until RobotInit() exits. Code in RobotInit() that
   * waits for enable will cause the robot to never indicate that the code is
   * ready, causing the robot to be bypassed in a match.
   */
  virtual void RobotInit();

  /**
   * Disabled should go here.
   *
   * Programmers should override this method to run code that should run while
   * the field is disabled.
   */
  virtual void Disabled();

  /**
   * Autonomous should go here.
   *
   * Programmers should override this method to run code that should run while
   * the field is in the autonomous period. This will be called once each time
   * the robot enters the autonomous state.
   */
  virtual void Autonomous();

  /**
   * Operator control (tele-operated) code should go here.
   *
   * Programmers should override this method to run code that should run while
   * the field is in the Operator Control (tele-operated) period. This is called
   * once each time the robot enters the teleop state.
   */
  virtual void OperatorControl();

  /**
   * Test program should go here.
   *
   * Programmers should override this method to run code that executes while the
   * robot is in test mode. This will be called once whenever the robot enters
   * test mode
   */
  virtual void Test();

  /**
   * Robot main program for free-form programs.
   *
   * This should be overridden by user subclasses if the intent is to not use
   * the Autonomous() and OperatorControl() methods. In that case, the program
   * is responsible for sensing when to run the autonomous and operator control
   * functions in their program.
   *
   * This method will be called immediately after the constructor is called. If
   * it has not been overridden by a user subclass (i.e. the default version
   * runs), then the Autonomous() and OperatorControl() methods will be called.
   */
  virtual void RobotMain();

 protected:
  WPI_DEPRECATED(
      "WARNING: While it may look like a good choice to use for your code if "
      "you're inexperienced, don't. Unless you know what you are doing, "
      "complex code will be much more difficult under this system. Use "
      "TimedRobot or Command-Based instead.")
  SampleRobot();
  virtual ~SampleRobot() = default;

  SampleRobot(SampleRobot&&) = default;
  SampleRobot& operator=(SampleRobot&&) = default;

 private:
  bool m_robotMainOverridden = true;
};

}  // namespace frc
