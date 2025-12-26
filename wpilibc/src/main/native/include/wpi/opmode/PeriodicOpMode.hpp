// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <chrono>
#include <functional>
#include <vector>

#include "wpi/framework/OpModeRobot.hpp"
#include "wpi/hal/Notifier.h"
#include "wpi/hal/Types.h"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/system/Watchdog.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/priority_queue.hpp"

namespace wpi {

/**
 * An opmode structure for periodic operation. This base class implements a loop
 * that runs one or more functions periodically (on a set time interval aka loop
 * period). The primary periodic callback function is the Periodic() function;
 * the time interval for this callback is 20 ms by default, but may be changed
 * via passing a different time interval to the constructor. Additional periodic
 * callbacks with different intervals can be added using the AddPeriodic() set
 * of functions.
 *
 * Lifecycle:
 *
 * - Constructed when opmode selected on driver station
 *
 * - DisabledPeriodic() called periodically as long as DS is disabled. Note
 *   this is not called on a set time interval (it does not use the same time
 *   interval as Periodic())
 *
 * - When DS transitions from disabled to enabled, Start() is called once
 *
 * - While DS is enabled, Periodic() is called periodically on the time interval
 *   set by the constructor, and additional periodic callbacks added via
 *   AddPeriodic() are called periodically on their set time intervals
 *
 * - When DS transitions from enabled to disabled, or a different opmode is
 *   selected on the driver station when the DS is enabled, End() is called,
 *   followed by the object being destroyed; the object is not reused
 *
 * - If a different opmode is selected on the driver station when the DS is
 *   disabled, the object is destroyed (without End() being called); the object
 *   is not reused
 */
class PeriodicOpMode : public OpMode {
 public:
  /** Default loop period. */
  static constexpr auto kDefaultPeriod = 20_ms;

 protected:
  /**
   * Constructor. Periodic opmodes may specify the period used for the
   * Periodic() function.
   *
   * @param robot   reference to the robot instance
   * @param period period for callbacks to the Periodic() function
   */
  explicit PeriodicOpMode(wpi::OpModeRobotBase& robot,
                          wpi::units::second_t period = kDefaultPeriod);

 public:
  ~PeriodicOpMode() override;

  /**
   * Called periodically while the opmode is selected on the DS (robot is
   * disabled).
   */
  void DisabledPeriodic() override {}

  /**
   * Called a single time when the robot transitions from disabled to enabled.
   * This is called prior to Periodic() being called.
   */
  virtual void Start() {}

  /** Called periodically while the robot is enabled. */
  virtual void Periodic() = 0;

  /**
   * Called a single time when the robot transitions from enabled to disabled,
   * or just before the destructor is called if a different opmode is selected
   * while the robot is enabled.
   */
  virtual void End() {}

  /**
   * Return the system clock time in microseconds for the start of the current
   * periodic loop. This is in the same time base as Timer.getFPGATimestamp(),
   * but is stable through a loop. It is updated at the beginning of every
   * periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current
   * periodic function.
   */
  int64_t GetLoopStartTime() const { return m_loopStartTimeUs; }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * This is scheduled on the same Notifier as Periodic(), so Periodic() and the
   * callback run synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period   The period at which to run the callback.
   * @param offset   The offset from the common starting time. This is useful
   *                 for scheduling a callback in a different timeslot relative
   *                 to TimedRobot.
   */
  void AddPeriodic(std::function<void()> callback, wpi::units::second_t period,
                   wpi::units::second_t offset = 0_s);

  /**
   * Gets time period between calls to Periodic() functions.
   */
  wpi::units::second_t GetPeriod() const { return m_period; }

  /**
   * Prints list of epochs added so far and their times.
   */
  void PrintWatchdogEpochs();

 protected:
  /** Loop function. */
  void LoopFunc();

 public:
  // implements OpMode interface
  void OpModeRun(int64_t opModeId) final;

  void OpModeStop() final;

 private:
  class Callback {
   public:
    std::function<void()> func;
    std::chrono::microseconds period;
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

    bool operator>(const Callback& rhs) const {
      return expirationTime > rhs.expirationTime;
    }
  };

  int64_t m_opModeId;
  bool m_running = true;

  wpi::hal::Handle<HAL_NotifierHandle, HAL_DestroyNotifier> m_notifier;
  std::chrono::microseconds m_startTime;
  int64_t m_loopStartTimeUs = 0;
  wpi::units::second_t m_period;
  Watchdog m_watchdog;

  wpi::util::priority_queue<Callback, std::vector<Callback>,
                            std::greater<Callback>>
      m_callbacks;

  wpi::OpModeRobotBase& m_robot;

  void PrintLoopOverrunMessage();
};

}  // namespace wpi
