// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <units/math.h>
#include <units/time.h>

namespace frc2 {

/**
 * The TimedCommandRobot class extends the {@link TimedRobot} class in order to
 * ease functionality for Command Based Programming.
 */
class TimedCommandRobot : public frc::TimedRobot {
 private:
  static constexpr auto kSchedulerOffset = 5_ms;

 public:
  /**
   * Constructor for TimedCommandRobot.
   *
   * @param mainPeriod periodic rate of the Main Robot Loop in seconds.
   * @param schedulerPeriod periodic rate of the CommandScheduler in seconds.
   */
  TimedCommandRobot(units::second_t mainPeriod,
                    units::second_t schedulerPeriod);

  /**
   * Constructor for TimedCommandRobot.
   *
   * @param period period to set for both the CommandScheduler and the Main
   * Robot Loop in seconds. Defaults to default periodic rate.
   */
  explicit TimedCommandRobot(units::second_t period = kDefaultPeriod);
};
}  // namespace frc2
