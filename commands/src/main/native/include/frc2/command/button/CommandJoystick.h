// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/Joystick.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
/**
 * A subclass of {@link Joystick} with {@link Trigger} factories for
 * command-based.
 *
 * @see Joystick
 */
class CommandJoystick : public frc::Joystick {
 public:
  using Joystick::Joystick;

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the button's digital signal attached
   * to the given loop.
   */
  class Trigger Button(
      int button, frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the trigger button's digital signal
   * attached to the given loop.
   */
  class Trigger Trigger(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the top button's digital signal
   * attached to the given loop.
   */
  class Trigger Top(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;
};
}  // namespace frc2
