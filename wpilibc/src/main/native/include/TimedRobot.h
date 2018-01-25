/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "IterativeRobotBase.h"
#include "Notifier.h"

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
class TimedRobot : public IterativeRobotBase {
 public:
  static constexpr double kDefaultPeriod = 0.02;

  void StartCompetition() override;

  void SetPeriod(double seconds);

 protected:
  TimedRobot();
  virtual ~TimedRobot();

 private:
  double m_period = kDefaultPeriod;

  // Prevents loop from starting if user calls SetPeriod() in RobotInit()
  bool m_startLoop = false;

  std::unique_ptr<Notifier> m_loop;
};

}  // namespace frc
