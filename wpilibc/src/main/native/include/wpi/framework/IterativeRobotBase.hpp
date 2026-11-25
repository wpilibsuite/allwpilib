// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/framework/RobotBase.hpp"
#include "wpi/system/Watchdog.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Alert.hpp"

namespace wpi {

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
 * DriverStationConnected() -- provide for initialization the first time the DS
 * is connected
 *
 * Start() functions -- each of the following functions is called once when the
 * appropriate mode starts:
 *
 * \li DisabledStart() -- called each and every time disabled mode starts
 * \li AutonomousStart() -- called each and every time autonomous mode starts
 * \li TeleopStart() -- called each and every time teleoperated mode starts
 * \li UtilityStart() -- called each and every time utility mode starts
 *
 * Periodic() functions -- each of these functions is called on an interval:
 *
 * \li RobotPeriodic()
 * \li DisabledPeriodic()
 * \li AutonomousPeriodic()
 * \li TeleopPeriodic()
 * \li UtilityPeriodic()
 *
 * Order within one loop iteration:
 *
 * 1. DriverStationConnected(), once, the first time the DS is connected
 * 2. The end function of the previous mode and the start function of the new
 *    mode, only on a mode change
 * 3. The periodic function for the current mode (DisabledPeriodic(),
 *    AutonomousPeriodic(), TeleopPeriodic(), or UtilityPeriodic())
 * 4. RobotPeriodic()
 * 5. SimulationPeriodic(), only in simulation
 *
 * The mode-specific periodic function runs before RobotPeriodic(). Exactly one
 * mode-specific periodic function runs per iteration.
 *
 * End() functions -- each of the following functions is called once when the
 * appropriate mode ends:
 *
 * \li DisabledEnd() -- called each and every time disabled mode ends
 * \li AutonomousEnd() -- called each and every time autonomous mode ends
 * \li TeleopEnd() -- called each and every time teleoperated mode ends
 * \li UtilityEnd() -- called each and every time utility mode ends
 */
class IterativeRobotBase : public RobotBase {
 public:
  /**
   * Code that needs to know the DS state should go here.
   *
   * Users should override this method for initialization that needs to occur
   * after the DS is connected, such as needing the alliance information.
   */
  virtual void DriverStationConnected();

  /**
   * Start code for simulation should go here.
   *
   * Users should override this method for default Robot-wide simulation
   * related initialization which will be called when the robot is first
   * started. It will be called exactly one time after the robot class
   * constructor is called only when the robot is in simulation.
   */
  virtual void SimulationStart();

  /**
   * Start code for disabled mode should go here.
   *
   * Users should override this method for code which will be called each time
   * disabled mode starts.
   */
  virtual void DisabledStart();

  /**
   * Start code for autonomous mode should go here.
   *
   * Users should override this method for code which will be called each time
   * autonomous mode starts.
   */
  virtual void AutonomousStart();

  /**
   * Start code for teleoperated mode should go here.
   *
   * Users should override this method for code which will be called each time
   * teleoperated mode starts.
   */
  virtual void TeleopStart();

  /**
   * Start code for utility mode should go here.
   *
   * Users should override this method for code which will be called each time
   * utility mode starts.
   */
  virtual void UtilityStart();

  /**
   * Periodic code for all modes should go here.
   *
   * This function is called each time a new packet is received from the driver
   * station.
   *
   * Runs after the current mode's periodic function (DisabledPeriodic(),
   * AutonomousPeriodic(), TeleopPeriodic(), or UtilityPeriodic()) and before
   * SimulationPeriodic().
   */
  virtual void RobotPeriodic();

  /**
   * Periodic simulation code should go here.
   *
   * This function is called in a simulated robot after user code executes.
   * Within a loop iteration it runs last, after the current mode's periodic
   * function and RobotPeriodic().
   */
  virtual void SimulationPeriodic();

  /**
   * Periodic code for disabled mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in disabled
   * mode.
   *
   * Runs before RobotPeriodic() and SimulationPeriodic(). Never runs in the
   * same iteration as another mode's periodic function.
   */
  virtual void DisabledPeriodic();

  /**
   * Periodic code for autonomous mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in
   * autonomous mode.
   *
   * Runs before RobotPeriodic() and SimulationPeriodic().
   */
  virtual void AutonomousPeriodic();

  /**
   * Periodic code for teleoperated mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in teleop
   * mode.
   *
   * Runs before RobotPeriodic() and SimulationPeriodic().
   */
  virtual void TeleopPeriodic();

  /**
   * Periodic code for utility mode should go here.
   *
   * Users should override this method for code which will be called each time a
   * new packet is received from the driver station and the robot is in utility
   * mode.
   *
   * Runs before RobotPeriodic() and SimulationPeriodic().
   */
  virtual void UtilityPeriodic();

  /**
   * End code for disabled mode should go here.
   *
   * Users should override this method for code which will be called each time
   * disabled mode ends.
   */
  virtual void DisabledEnd();

  /**
   * End code for autonomous mode should go here.
   *
   * Users should override this method for code which will be called each time
   * autonomous mode ends.
   */
  virtual void AutonomousEnd();

  /**
   * End code for teleoperated mode should go here.
   *
   * Users should override this method for code which will be called each time
   * teleoperated mode ends.
   */
  virtual void TeleopEnd();

  /**
   * End code for utility mode should go here.
   *
   * Users should override this method for code which will be called each time
   * utility mode ends.
   */
  virtual void UtilityEnd();

  /**
   * Gets time period between calls to Periodic() functions.
   */
  wpi::units::second_t GetPeriod() const;

  /**
   * Prints list of epochs added so far and their times.
   */
  void PrintWatchdogEpochs();

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period.
   */
  explicit IterativeRobotBase(wpi::units::second_t period);

  ~IterativeRobotBase() override = default;

 protected:
  IterativeRobotBase(IterativeRobotBase&&) = default;
  IterativeRobotBase& operator=(IterativeRobotBase&&) = default;

  /**
   * Loop function.
   */
  void LoopFunc();

 private:
  int m_lastMode = -1;
  wpi::units::second_t m_period;
  wpi::util::Alert m_loopOverrunAlert;
  Watchdog m_watchdog;
  bool m_calledDsConnected = false;
};

}  // namespace wpi
