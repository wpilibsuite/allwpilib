// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <thread>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Main.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "frc/Errors.h"
#include "frc/RuntimeType.h"

namespace frc {

int RunHALInitialization();

namespace impl {
#ifndef __FRC_ROBORIO__
void ResetMotorSafety();
#endif

template <class Robot>
void RunRobot(wpi::mutex& m, Robot** robot) {
  try {
    static Robot theRobot;
    {
      std::scoped_lock lock{m};
      *robot = &theRobot;
    }
    theRobot.StartCompetition();
  } catch (const frc::RuntimeError& e) {
    e.Report();
    FRC_ReportError(
        err::Error,
        "The robot program quit unexpectedly."
        " This is usually due to a code error.\n"
        "  The above stacktrace can help determine where the error occurred.\n"
        "  See https://wpilib.org/stacktrace for more information.\n");
    throw;
  } catch (const std::exception& e) {
    HAL_SendError(1, err::Error, 0, e.what(), "", "", 1);
    throw;
  }
}

}  // namespace impl

template <class Robot>
int StartRobot() {
  int halInit = RunHALInitialization();
  if (halInit != 0) {
    return halInit;
  }

  static wpi::mutex m;
  static wpi::condition_variable cv;
  static Robot* robot = nullptr;
  static bool exited = false;

  if (HAL_HasMain()) {
    std::thread thr([] {
      try {
        impl::RunRobot<Robot>(m, &robot);
      } catch (...) {
        HAL_ExitMain();
        {
          std::scoped_lock lock{m};
          robot = nullptr;
          exited = true;
        }
        cv.notify_all();
        throw;
      }

      HAL_ExitMain();
      {
        std::scoped_lock lock{m};
        robot = nullptr;
        exited = true;
      }
      cv.notify_all();
    });

    HAL_RunMain();

    // signal loop to exit
    if (robot) {
      robot->EndCompetition();
    }

    // prefer to join, but detach to exit if it doesn't exit in a timely manner
    using namespace std::chrono_literals;
    std::unique_lock lock{m};
    if (cv.wait_for(lock, 1s, [] { return exited; })) {
      thr.join();
    } else {
      thr.detach();
    }
  } else {
    impl::RunRobot<Robot>(m, &robot);
  }

#ifndef __FRC_ROBORIO__
  frc::impl::ResetMotorSafety();
#endif
  HAL_Shutdown();

  return 0;
}

/**
 * Implement a Robot Program framework. The RobotBase class is intended to be
 * subclassed to create a robot program. The user must implement
 * StartCompetition() which will be called once and is not expected to exit. The
 * user must also implement EndCompetition(), which signals to the code in
 * StartCompetition() that it should exit.
 *
 * It is not recommended to subclass this class directly - instead subclass
 * IterativeRobotBase or TimedRobot.
 */
class RobotBase {
 public:
  /**
   * Determine if the Robot is currently enabled.
   *
   * @return True if the Robot is currently enabled by the Driver Station.
   */
  bool IsEnabled() const;

  /**
   * Determine if the Robot is currently disabled.
   *
   * @return True if the Robot is currently disabled by the Driver Station.
   */
  bool IsDisabled() const;

  /**
   * Determine if the robot is currently in Autonomous mode.
   *
   * @return True if the robot is currently operating Autonomously as determined
   *         by the Driver Station.
   */
  bool IsAutonomous() const;

  /**
   * Determine if the robot is currently in Autonomous mode and enabled.
   *
   * @return True if the robot us currently operating Autonomously while enabled
   * as determined by the Driver Station.
   */
  bool IsAutonomousEnabled() const;

  /**
   * Determine if the robot is currently in Operator Control mode.
   *
   * @return True if the robot is currently operating in Tele-Op mode as
   *         determined by the Driver Station.
   */
  bool IsTeleop() const;

  /**
   * Determine if the robot is current in Operator Control mode and enabled.
   *
   * @return True if the robot is currently operating in Tele-Op mode while
   * enabled as determined by the Driver Station.
   */
  bool IsTeleopEnabled() const;

  /**
   * Determine if the robot is currently in Test mode.
   *
   * @return True if the robot is currently running in Test mode as determined
   * by the Driver Station.
   */
  bool IsTest() const;

  /**
   * Determine if the robot is current in Test mode and enabled.
   *
   * @return True if the robot is currently operating in Test mode while
   * enabled as determined by the Driver Station.
   */
  bool IsTestEnabled() const;

  /**
   * Returns the main thread ID.
   *
   * @return The main thread ID.
   */
  static std::thread::id GetThreadId();

  /**
   * Start the main robot code. This function will be called once and should not
   * exit until signalled by EndCompetition()
   */
  virtual void StartCompetition() = 0;

  /** Ends the main loop in StartCompetition(). */
  virtual void EndCompetition() = 0;

  /**
   * Get the current runtime type.
   *
   * @return Current runtime type.
   */
  static RuntimeType GetRuntimeType();

  /**
   * Get if the robot is real.
   *
   * @return If the robot is running in the real world.
   */
  static constexpr bool IsReal() {
#ifdef __FRC_ROBORIO__
    return true;
#else
    return false;
#endif
  }

  /**
   * Get if the robot is a simulation.
   *
   * @return If the robot is running in simulation.
   */
  static constexpr bool IsSimulation() {
#ifdef __FRC_ROBORIO__
    return false;
#else
    return true;
#endif
  }

  /**
   * Constructor for a generic robot program.
   *
   * User code can be placed in the constructor that runs before the
   * Autonomous or Operator Control period starts. The constructor will run to
   * completion before Autonomous is entered.
   *
   * This must be used to ensure that the communications code starts. In the
   * future it would be nice to put this code into it's own task that loads on
   * boot so ensure that it runs.
   */
  RobotBase();

  virtual ~RobotBase() = default;

 protected:
  RobotBase(RobotBase&&) = default;
  RobotBase& operator=(RobotBase&&) = default;

  static std::thread::id m_threadId;
};

}  // namespace frc
