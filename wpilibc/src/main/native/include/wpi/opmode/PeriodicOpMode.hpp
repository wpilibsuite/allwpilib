// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <functional>
#include <utility>
#include <vector>

#include "wpi/internal/PeriodicPriorityQueue.hpp"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/units/time.hpp"

namespace wpi {

/**
 * An opmode structure for periodic operation. This base class implements a loop
 * that runs one or more functions periodically (on a set time interval aka loop
 * period). The primary periodic callback function is the abstract Periodic()
 * function. Additional periodic callbacks with different intervals can be added
 * using the AddPeriodic() set of functions.
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
 *   set by the robot framework, and additional periodic callbacks added via
 *   AddPeriodic() are called periodically on their set time intervals
 *
 * - When DS transitions from enabled to disabled, or a different opmode is
 *   selected on the driver station when the DS is enabled, End() is called,
 *   followed by Close(); the object is not reused
 *
 * - If a different opmode is selected on the driver station when the DS is
 *   disabled, only Close() is called; the object is not reused
 */
class PeriodicOpMode : public OpMode {
 protected:
  /**
   * Constructor.
   */
  PeriodicOpMode();

 public:
  ~PeriodicOpMode() override = default;

  /**
   * Called periodically while the opmode is selected on the DS (robot is
   * disabled).
   */
  void DisabledPeriodic() override {}

  /**
   * Called a single time when the robot transitions from disabled to enabled.
   * This is called prior to Periodic() being called.
   */
  void Start() override {}

  /**
   * Called periodically while the robot is enabled. The framework calls this
   * at OpModeRobot's configured loop period.
   */
  void Periodic() override {}

  /**
   * Called a single time when the robot transitions from enabled to disabled,
   * or just before Close() is called if a different opmode is selected while
   * the robot is enabled.
   */
  void End() override {}

  /**
   * Returns the set of additional periodic callbacks registered via
   * AddPeriodic(). These are executed by the robot framework while the opmode
   * is enabled, in addition to the primary Periodic() callback.
   *
   * @return The vector of additional periodic callbacks.
   */
  std::vector<wpi::internal::PeriodicPriorityQueue::Callback> GetCallbacks()
      override {
    return m_callbacks;
  }

  /**
   * Add a callback to run at a specific period.
   *
   * This is scheduled on the same Notifier as Periodic(), so Periodic() and
   * the callback run synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period   The period at which to run the callback.
   */
  void AddPeriodic(std::function<void()> callback,
                   wpi::units::second_t period) {
    AddPeriodic(std::move(callback), period, period);
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * This is scheduled on the same Notifier as Periodic(), so Periodic() and
   * the callback run synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period   The period at which to run the callback.
   * @param offset   The offset from the common starting time. This is useful
   *                 for scheduling a callback in a different timeslot relative
   *                 to TimedRobot.
   */
  void AddPeriodic(std::function<void()> callback, wpi::units::second_t period,
                   wpi::units::second_t offset);

 private:
  std::vector<wpi::internal::PeriodicPriorityQueue::Callback> m_callbacks;
  std::chrono::microseconds m_startTime;
};

}  // namespace wpi
