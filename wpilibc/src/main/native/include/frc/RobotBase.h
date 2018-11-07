/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <thread>

#include <wpi/raw_ostream.h>

#include "frc/Base.h"

namespace frc {

class DriverStation;

int RunHALInitialization();

template <class Robot>
int StartRobot() {
  int halInit = RunHALInitialization();
  if (halInit != 0) {
    return halInit;
  }
  static Robot robot;
  robot.StartCompetition();

  return 0;
}

#define START_ROBOT_CLASS(_ClassName_)                                 \
  WPI_DEPRECATED("Call frc::StartRobot<" #_ClassName_                  \
                 ">() in your own main() instead of using the "        \
                 "START_ROBOT_CLASS(" #_ClassName_ ") macro.")         \
  int StartRobotClassImpl() { return frc::StartRobot<_ClassName_>(); } \
  int main() { return StartRobotClassImpl(); }

/**
 * Implement a Robot Program framework.
 *
 * The RobotBase class is intended to be subclassed by a user creating a robot
 * program. Overridden Autonomous() and OperatorControl() methods are called at
 * the appropriate time as the match proceeds. In the current implementation,
 * the Autonomous code will run to completion before the OperatorControl code
 * could start. In the future the Autonomous code might be spawned as a task,
 * then killed at the end of the Autonomous period.
 */
class RobotBase {
 public:
  /**
   * Determine if the Robot is currently enabled.
   *
   * @return True if the Robot is currently enabled by the field controls.
   */
  bool IsEnabled() const;

  /**
   * Determine if the Robot is currently disabled.
   *
   * @return True if the Robot is currently disabled by the field controls.
   */
  bool IsDisabled() const;

  /**
   * Determine if the robot is currently in Autonomous mode.
   *
   * @return True if the robot is currently operating Autonomously as determined
   *         by the field controls.
   */
  bool IsAutonomous() const;

  /**
   * Determine if the robot is currently in Operator Control mode.
   *
   * @return True if the robot is currently operating in Tele-Op mode as
   *         determined by the field controls.
   */
  bool IsOperatorControl() const;

  /**
   * Determine if the robot is currently in Test mode.
   *
   * @return True if the robot is currently running tests as determined by the
   *         field controls.
   */
  bool IsTest() const;

  /**
   * Indicates if new data is available from the driver station.
   *
   * @return Has new data arrived over the network since the last time this
   *         function was called?
   */
  bool IsNewDataAvailable() const;

  /**
   * Gets the ID of the main robot thread.
   */
  static std::thread::id GetThreadId();

  virtual void StartCompetition() = 0;

  static constexpr bool IsReal() {
#ifdef __FRC_ROBORIO__
    return true;
#else
    return false;
#endif
  }

  static constexpr bool IsSimulation() { return !IsReal(); }

 protected:
  /**
   * Constructor for a generic robot program.
   *
   * User code should be placed in the constructor that runs before the
   * Autonomous or Operator Control period starts. The constructor will run to
   * completion before Autonomous is entered.
   *
   * This must be used to ensure that the communications code starts. In the
   * future it would be nice to put this code into it's own task that loads on
   * boot so ensure that it runs.
   */
  RobotBase();

  virtual ~RobotBase();

  // m_ds isn't moved in these because DriverStation is a singleton; every
  // instance of RobotBase has a reference to the same object.
  RobotBase(RobotBase&&);
  RobotBase& operator=(RobotBase&&);

  DriverStation& m_ds;

  static std::thread::id m_threadId;
};

}  // namespace frc
