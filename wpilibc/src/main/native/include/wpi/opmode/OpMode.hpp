// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "wpi/internal/PeriodicPriorityQueue.hpp"

namespace wpi {

/**
 * Top-level interface for opmode classes. Users should generally extend one of
 * the abstract implementations of this interface (e.g. PeriodicOpMode) rather
 * than directly implementing this interface.
 *
 * <p><b>Lifecycle</b>:
 * <ul>
 *   <li>constructed when opmode selected on driver station
 *   <li>DisabledPeriodic() called periodically as long as DS is disabled
 *   <li>when DS transitions from disabled to enabled, Start() is called once
 *   <li>while DS is enabled, Periodic() is called periodically and additional
 *       periodic callbacks added via GetCallbacks() are called periodically
 *   <li>when DS transitions from enabled to disabled, or a different opmode is
 *       selected while enabled, End() is called, followed by Close(); the
 *       object is not reused
 *   <li>if a different opmode is selected while disabled, only Close() is
 *       called; the object is not reused
 * </ul>
 */
class OpMode {
 public:
  /**
   * The object is destroyed when the opmode is no longer selected on the DS or
   * after an enabled run ends. The object will not be reused after the
   * destructor is called.
   */
  virtual ~OpMode() = default;

  /**
   * This function is called periodically while the opmode is selected on the
   * DS (robot is disabled). Code that should only run once when the opmode is
   * selected should go in the opmode constructor.
   */
  virtual void DisabledPeriodic() {}

  /** Called once when this opmode transitions to enabled. */
  virtual void Start() {}

  /**
   * This function is called periodically while the opmode is enabled.
   */
  virtual void Periodic() {}

  /**
   * This function is called when the robot disables or switches opmodes while
   * this opmode is enabled. Implementations should stop blocking work
   * promptly.
   */
  virtual void End() {}

  /**
   * Returns a vector of custom periodic callbacks to be executed while the
   * opmode is enabled.
   *
   * <p>This method allows opmodes to register arbitrary periodic callbacks
   * with custom execution intervals. The callbacks are executed by the robot
   * framework at their scheduled times, in addition to the primary Periodic()
   * callback.
   *
   * @return A vector of custom callbacks to execute, or an empty vector if no
   *         custom callbacks are needed. The default implementation returns an
   *         empty vector.
   */
  virtual std::vector<wpi::internal::PeriodicPriorityQueue::Callback>
  GetCallbacks() {
    return {};
  }
};

}  // namespace wpi
