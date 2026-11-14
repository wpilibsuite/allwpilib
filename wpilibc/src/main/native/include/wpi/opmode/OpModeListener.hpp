// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/opmode/OpMode.hpp"

namespace wpi {

/**
 * Receives notifications about the lifecycle of the opmodes run by an
 * OpModeRobot.
 *
 * Listeners are registered with OpModeRobotBase::RegisterListener(). All
 * methods have empty default implementations, so implementations only need to
 * override the events they care about.
 *
 * Callbacks run synchronously on the robot loop thread, so they should return
 * quickly. Select and start callbacks run in registration order; stop and close
 * callbacks run in reverse registration order, so a listener's teardown mirrors
 * its setup relative to other listeners.
 */
class OpModeListener {
 public:
  virtual ~OpModeListener() = default;

  /**
   * Called after an opmode has been selected on the Driver Station and
   * constructed, before it is started. This runs once per selection, whether or
   * not the robot is enabled.
   *
   * @param opModeName the name the opmode was registered under
   * @param opMode the newly constructed opmode
   */
  virtual void OnOpModeSelect(std::string_view opModeName, OpMode& opMode) {}

  /**
   * Called after the opmode's Start() has run, when the robot is enabled with
   * the opmode selected.
   *
   * @param opModeName the name the opmode was registered under
   * @param opMode the opmode that was started
   */
  virtual void OnOpModeStart(std::string_view opModeName, OpMode& opMode) {}

  /**
   * Called before the opmode's End() runs. This is only called for opmodes that
   * were started.
   *
   * @param opModeName the name the opmode was registered under
   * @param opMode the opmode that is about to end
   */
  virtual void OnOpModeStop(std::string_view opModeName, OpMode& opMode) {}

  /**
   * Called before the opmode is destroyed. This is called for every opmode that
   * was constructed, including ones that were selected but never started.
   *
   * @param opModeName the name the opmode was registered under
   * @param opMode the opmode that is about to be closed
   */
  virtual void OnOpModeClose(std::string_view opModeName, OpMode& opMode) {}
};

}  // namespace wpi
