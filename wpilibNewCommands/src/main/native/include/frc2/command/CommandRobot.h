// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <units/math.h>
#include <units/time.h>

namespace frc2 {

/**
 * The CommandRobot class extends the {@link TimedRobot} class in order to ease
 * functionality for Command Based Programming.
 */
class CommandRobot : public frc::TimedRobot {
 private:
  static constexpr auto kSchedulerOffset = 5_ms;

 public:
  /**
   * Constructor for CommandRobot.
   *
   * @param mainPeriod periodic rate of the Main Robot Loop in ms.
   * @param schedulerPeriod periodic rate of the CommandScheduler in ms.
   */
  CommandRobot(units::second_t mainPeriod, units::second_t schedulerPeriod);

  /**
   * Constructor for CommandRobot.
   *
   * @param period period to set for both the CommandScheduler and the Main
   * Robot Loop in ms.
   */
  explicit CommandRobot(units::second_t period);

  /**
   * Set both the CommandScheduler and the Main Robot Loop to run at the default
   * periodic rate
   */
  CommandRobot();
};
}  // namespace frc2
