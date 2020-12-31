// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <hal/Types.h>
#include <units/math.h>
#include <units/time.h>
#include <wpi/deprecated.h>
#include <wpi/priority_queue.h>

#include "frc/ErrorBase.h"
#include "frc/IterativeRobotBase.h"
#include "frc2/Timer.h"

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
  static constexpr units::second_t kDefaultPeriod = 20_ms;

  /**
   * Provide an alternate "main loop" via StartCompetition().
   */
  void StartCompetition() override;

  /**
   * Ends the main loop in StartCompetition().
   */
  void EndCompetition() override;

  /**
   * Get the time period between calls to Periodic() functions.
   */
  units::second_t GetPeriod() const;

  /**
   * Constructor for TimedRobot.
   *
   * @deprecated use unit safe constructor instead.
   * TimedRobot(units::second_t period = kDefaultPeriod)
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
  explicit TimedRobot(units::second_t period = kDefaultPeriod);

  ~TimedRobot() override;

  TimedRobot(TimedRobot&&) = default;
  TimedRobot& operator=(TimedRobot&&) = default;

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
  void AddPeriodic(std::function<void()> callback, units::second_t period,
                   units::second_t offset = 0_s);

 private:
  class Callback {
   public:
    std::function<void()> func;
    units::second_t period;
    units::second_t expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func      The callback to run.
     * @param startTime The common starting point for all callback scheduling.
     * @param period    The period at which to run the callback.
     * @param offset    The offset from the common starting time.
     */
    Callback(std::function<void()> func, units::second_t startTime,
             units::second_t period, units::second_t offset)
        : func{std::move(func)},
          period{period},
          expirationTime{
              startTime + offset +
              units::math::floor((frc2::Timer::GetFPGATimestamp() - startTime) /
                                 period) *
                  period +
              period} {}

    bool operator>(const Callback& rhs) const {
      return expirationTime > rhs.expirationTime;
    }
  };

  hal::Handle<HAL_NotifierHandle> m_notifier;
  units::second_t m_startTime;

  wpi::priority_queue<Callback, std::vector<Callback>, std::greater<Callback>>
      m_callbacks;
};

}  // namespace frc
