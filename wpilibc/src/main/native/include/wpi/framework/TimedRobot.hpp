// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "wpi/framework/IterativeRobotBase.hpp"
#include "wpi/framework/PeriodicPriorityQueue.hpp"
#include "wpi/hal/Notifier.hpp"
#include "wpi/hal/Types.hpp"
#include "wpi/units/frequency.hpp"
#include "wpi/units/time.hpp"

namespace wpi {

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
  /// Default loop period.
  static constexpr auto kDefaultPeriod = 20_ms;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   */
  void StartCompetition() override;

  /**
   * Ends the main loop in StartCompetition().
   */
  void EndCompetition() override;

  /**
   * Constructor for TimedRobot.
   *
   * @param period The period of the robot loop function.
   */
  explicit TimedRobot(wpi::units::second_t period = kDefaultPeriod);

  /**
   * Constructor for TimedRobot.
   *
   * @param frequency The frequency of the robot loop function.
   */
  explicit TimedRobot(wpi::units::hertz_t frequency);

  TimedRobot(TimedRobot&&) = default;
  TimedRobot& operator=(TimedRobot&&) = default;

  ~TimedRobot() override;

  /**
   * Return the system clock time in microseconds for the start of the current
   * periodic loop. This is in the same time base as Timer.GetFPGATimestamp(),
   * but is stable through a loop. It is updated at the beginning of every
   * periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current
   * periodic function.
   */
  uint64_t GetLoopStartTime();

  /**
   * Returns the queue of periodic callbacks. While it is possible to directly
   * modify this queue, it is recommended to use the AddPeriodic methods to add
   * periodic callbacks.
   *
   * @return the queue of periodic callbacks.
   */
  PeriodicPriorityQueue& GetCallbacks() { return m_callbacks; }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback
   * run synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period   The period at which to run the callback.
   * @param offset   The offset from the common starting time. This is useful
   *                 for scheduling a callback in a different timeslot relative
   *                 to TimedRobot.
   */
  void AddPeriodic(std::function<void()> callback, wpi::units::second_t period,
                   wpi::units::second_t offset = 0_s);

 protected:
  wpi::hal::Handle<HAL_NotifierHandle, HAL_DestroyNotifier> m_notifier;
  std::chrono::microseconds m_startTime;

 private:
  uint64_t m_loopStartTimeUs = 0;

  PeriodicPriorityQueue m_callbacks;
};

}  // namespace wpi
