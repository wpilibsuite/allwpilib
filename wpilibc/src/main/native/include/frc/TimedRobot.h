/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>
#include <units/units.h>

#include <wpi/deprecated.h>

#include "frc/ErrorBase.h"
#include "frc/IterativeRobotBase.h"

namespace frc {

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * The TimedRobot class is intended to be subclassed by a user creating a
 * robot program.
 *
 * Periodic() functions from the base class are called on an interval by a
 * Notifier instance.
 */
class TimedRobot : public IterativeRobotBase, public ErrorBase {
 public:
  static constexpr double kDefaultPeriod = 0.02;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   */
  void StartCompetition() override;

  /**
   * Get the time period between calls to Periodic() functions.
   */
  double GetPeriod() const;

  /**
   * Constructor for TimedRobot.
   *
   * @param period Period in seconds.
   */
  WPI_DEPRECATED("Use constructor with unit-safety instead.")
  explicit TimedRobot(double period);

  /**
   * Constructor for TimedRobot.
   *
   * @param period Period.
   */
  explicit TimedRobot(units::second_t period = units::second_t(kDefaultPeriod));

  ~TimedRobot() override;

  TimedRobot(TimedRobot&& rhs);
  TimedRobot& operator=(TimedRobot&& rhs);

 private:
  HAL_NotifierHandle m_notifier{0};

  // The absolute expiration time
  double m_expirationTime = 0;

  /**
   * Update the HAL alarm time.
   */
  void UpdateAlarm();
};

}  // namespace frc
