/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/Notifier.h>

#include "ErrorBase.h"
#include "IterativeRobotBase.h"

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

  void StartCompetition() override;

  void SetPeriod(double seconds);
  double GetPeriod() const;

 protected:
  TimedRobot();
  ~TimedRobot() override;

 private:
  // Prevents loop from starting if user calls SetPeriod() in RobotInit()
  bool m_startLoop = false;

  HAL_NotifierHandle m_notifier{0};

  // The absolute expiration time
  double m_expirationTime = 0;

  // The relative time
  double m_period = kDefaultPeriod;

  void UpdateAlarm();
};

}  // namespace frc
