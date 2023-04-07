// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/XboxController.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
/**
 * A version of {@link XboxController} with {@link Trigger} factories for
 * command-based.
 *
 * @see XboxController
 */
class CommandXboxController : public frc::XboxController {
 public:
  using XboxController::XboxController;

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the button's digital signal attached
   * to the given loop.
   */
  Trigger Button(int button,
                 frc::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the left bumper's digital signal
   * attached to the given loop.
   */
  Trigger LeftBumper(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the right bumper's digital signal
   * attached to the given loop.
   */
  Trigger RightBumper(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the left stick's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the left stick's digital signal
   * attached to the given loop.
   */
  Trigger LeftStick(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the right stick's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the right stick's digital signal
   * attached to the given loop.
   */
  Trigger RightStick(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the A button's digital signal
   * attached to the given loop.
   */
  Trigger A(frc::EventLoop* loop =
                CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the B button's digital signal
   * attached to the given loop.
   */
  Trigger B(frc::EventLoop* loop =
                CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the X button's digital signal
   * attached to the given loop.
   */
  Trigger X(frc::EventLoop* loop =
                CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the Y button's digital signal
   * attached to the given loop.
   */
  Trigger Y(frc::EventLoop* loop =
                CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the back button's digital signal
   * attached to the given loop.
   */
  Trigger Back(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                          .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the start button's digital signal
   * attached to the given loop.
   */
  Trigger Start(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the axis value of the left trigger.
   * The returned Trigger will be true when the axis value is greater than
   * {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned Trigger to be
   * true. This value should be in the range [0, 1] where 0 is the unpressed
   * state of the axis. Defaults to 0.5.
   * @param loop the event loop instance to attach the Trigger to. Defaults to
   * the CommandScheduler's default loop.
   * @return a Trigger instance that is true when the left trigger's axis
   * exceeds the provided threshold, attached to the given loop
   */
  Trigger LeftTrigger(double threshold = 0.5,
                      frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the axis value of the right trigger.
   * The returned Trigger will be true when the axis value is greater than
   * {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned Trigger to be
   * true. This value should be in the range [0, 1] where 0 is the unpressed
   * state of the axis. Defaults to 0.5.
   * @param loop the event loop instance to attach the Trigger to. Defaults to
   * the CommandScheduler's default loop.
   * @return a Trigger instance that is true when the right trigger's axis
   * exceeds the provided threshold, attached to the given loop
   */
  Trigger RightTrigger(
      double threshold = 0.5,
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;
};
}  // namespace frc2
