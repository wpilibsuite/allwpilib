/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>
#include <wpi/deprecated.h>

#include "frc/RobotBase.h"
#include "frc/Watchdog.h"

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
 *   - DisabledInit()   -- called each and every time disabled is entered from
 *                         another mode
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
   * Initialization code for disabled mode should go here.
   *
   * Users should override this method for initialization code which will be
   * called each time
   * the robot enters disabled mode.
   */
  virtual void DisabledInit();

  /**
   * Initialization code for autonomous mode should go here.
   *
   * Users should override this method for initialization code which will be
   * called each time the robot enters autonomous mode.
   */
  virtual void AutonomousInit();

  /**
   * Initialization code for teleop mode should go here.
   *
   * Users should override this method for initialization code which will be
   * called each time the robot enters teleop mode.
   */
  virtual void TeleopInit();

  /**
   * Initialization code for test mode should go here.
   *
   * Users should override this method for initialization code which will be
   * called each time the robot enters test mode.
   */
  virtual void TestInit();

  /**
   * Periodic code for all modes should go here.
   *
   * This function is called each time a new packet is received from the driver
   * station.
   */
  virtual void RobotPeriodic();

  /**
   * Periodic code for disabled mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in disabled
   * mode.
   */
  virtual void DisabledPeriodic();

  /**
   * Periodic code for autonomous mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in
   * autonomous mode.
   */
  virtual void AutonomousPeriodic();

  /**
   * Periodic code for teleop mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in teleop
   * mode.
   */
  virtual void TeleopPeriodic();

  /**
   * Periodic code for test mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in test
   * mode.
   */
  virtual void TestPeriodic();

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period in seconds.
   */
  WPI_DEPRECATED("Use ctor with unit-safety instead.")
  explicit IterativeRobotBase(double period);

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period.
   */
  explicit IterativeRobotBase(units::second_t period);

  virtual ~IterativeRobotBase() = default;

 protected:
  IterativeRobotBase(IterativeRobotBase&&) = default;
  IterativeRobotBase& operator=(IterativeRobotBase&&) = default;

  void LoopFunc();

  units::second_t m_period;

 private:
  enum class Mode { kNone, kDisabled, kAutonomous, kTeleop, kTest };

  Mode m_lastMode = Mode::kNone;
  Watchdog m_watchdog;

  void PrintLoopOverrunMessage();
};

}  // namespace frc
