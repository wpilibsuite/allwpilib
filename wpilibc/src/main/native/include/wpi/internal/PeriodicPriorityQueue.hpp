// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include "wpi/hal/Notifier.h"
#include "wpi/units/time.hpp"
#include "wpi/util/priority_queue.hpp"

namespace wpi::internal {

/**
 * A priority queue for scheduling periodic callbacks based on their next
 * execution time.
 *
 * <p>This class manages a collection of periodic callbacks that execute at
 * specified intervals. Callbacks are scheduled using FPGA timestamps and
 * automatically rescheduled after execution to maintain their periodic
 * behavior. The queue uses a priority heap to efficiently determine the next
 * callback to execute.
 *
 * <p>This is an internal scheduling primitive used by robot frameworks like
 * TimedRobot.
 */
class PeriodicPriorityQueue {
 public:
  /**
   * A periodic callback with scheduling metadata.
   *
   * Each callback tracks its target function, period, and next expiration
   * time. After execution, the expiration time is automatically advanced by
   * full periods to maintain precise timing even when execution is delayed.
   */
  class Callback {
   public:
    /** The function to execute when the callback fires. */
    std::function<void()> func;

    /** The period at which to run the callback. */
    std::chrono::microseconds period;

    /** The next scheduled execution time in FPGA timestamp microseconds. */
    std::chrono::microseconds expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func      The callback to run.
     * @param startTime The common starting point for all callback scheduling.
     * @param period    The period at which to run the callback.
     * @param offset    The offset from the common starting time.
     */
    Callback(std::function<void()> func, std::chrono::microseconds startTime,
             std::chrono::microseconds period,
             std::chrono::microseconds offset);

    /**
     * Construct a callback container using units-based period and offset.
     *
     * @param func      The callback to run.
     * @param startTime The common starting point for all callback scheduling.
     * @param period    The period at which to run the callback.
     * @param offset    The offset from the common starting time.
     */
    Callback(std::function<void()> func, std::chrono::microseconds startTime,
             wpi::units::second_t period, wpi::units::second_t offset);

    /**
     * Construct a callback container using units-based period.
     *
     * @param func      The callback to run.
     * @param startTime The common starting point for all callback scheduling.
     * @param period    The period at which to run the callback.
     */
    Callback(std::function<void()> func, std::chrono::microseconds startTime,
             wpi::units::second_t period);

    bool operator>(const Callback& rhs) const {
      return expirationTime > rhs.expirationTime;
    }

    bool operator==(const Callback& rhs) const {
      return expirationTime == rhs.expirationTime;
    }
  };

  /**
   * Adds a periodic callback to the queue.
   *
   * @param func      The callback to run.
   * @param startTime The common starting point for all callback scheduling.
   * @param period    The period at which to run the callback.
   */
  void Add(std::function<void()> func, std::chrono::microseconds startTime,
           std::chrono::microseconds period);

  /**
   * Adds a periodic callback to the queue.
   *
   * @param func      The callback to run.
   * @param startTime The common starting point for all callback scheduling.
   * @param period    The period at which to run the callback.
   * @param offset    The offset from the common starting time.
   */
  void Add(std::function<void()> func, std::chrono::microseconds startTime,
           std::chrono::microseconds period, std::chrono::microseconds offset);

  /**
   * Adds a periodic callback to the queue.
   *
   * @param func      The callback to run.
   * @param startTime The common starting point for all callback scheduling in
   *                  FPGA timestamp microseconds.
   * @param period    The period at which to run the callback.
   */
  void Add(std::function<void()> func, std::chrono::microseconds startTime,
           wpi::units::second_t period);

  /**
   * Adds a periodic callback to the queue.
   *
   * @param func      The callback to run.
   * @param startTime The common starting point for all callback scheduling in
   *                  FPGA timestamp microseconds.
   * @param period    The period at which to run the callback.
   * @param offset    The offset from the common starting time.
   */
  void Add(std::function<void()> func, std::chrono::microseconds startTime,
           wpi::units::second_t period, wpi::units::second_t offset);

  /**
   * Adds a pre-constructed callback to the queue.
   *
   * @param callback The callback to add.
   */
  void Add(Callback callback);

  /**
   * Removes a specific callback from the queue.
   *
   * @param callback The callback to remove.
   * @return true if the callback was found and removed, false otherwise.
   */
  bool Remove(const Callback& callback);

  /**
   * Removes all callbacks from the queue.
   */
  void Clear();

  /**
   * Executes all callbacks that are due, then waits for the next callback's
   * scheduled time.
   *
   * This method performs the following steps:
   * -# Retrieves the callback with the earliest expiration time from the queue
   * -# Sets a hardware notifier alarm to wait until that callback's expiration
   *    time
   * -# Blocks until the notifier signals or is interrupted
   * -# Executes the callback and reschedules it for its next period
   * -# Processes any additional callbacks that have become due during execution
   *
   * When rescheduling callbacks, this method automatically compensates for
   * execution delays by advancing the expiration time by the number of full
   * periods that have elapsed, ensuring callbacks maintain their scheduled
   * phase over time.
   *
   * @param notifier The HAL notifier handle to use for timing.
   * @return false if the notifier was destroyed (loop should exit), true
   *         otherwise.
   */
  bool RunCallbacks(HAL_NotifierHandle notifier);

  /**
   * Returns the underlying priority queue.
   */
  wpi::util::priority_queue<Callback, std::vector<Callback>, std::greater<>>&
  GetQueue() {
    return m_queue;
  }

  /**
   * Return the system clock time in microseconds for the start of the current
   * periodic loop. This is in the same time base as
   * Timer.getMonotonicTimeStamp(), but is stable through a loop. It is updated
   * at the beginning of every periodic callback (including the normal periodic
   * loop).
   *
   * @return Robot running time in microseconds, as of the start of the current
   * periodic function.
   */
  wpi::units::microsecond_t GetLoopStartTime() const { return m_loopStartTime; }

 private:
  wpi::util::priority_queue<Callback, std::vector<Callback>, std::greater<>>
      m_queue;

  wpi::units::microsecond_t m_loopStartTime{0};
};

}  // namespace wpi::internal
