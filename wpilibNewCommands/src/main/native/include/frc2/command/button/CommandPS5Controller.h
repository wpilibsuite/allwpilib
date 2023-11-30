// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/PS5Controller.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
/**
 * A version of {@link PS5Controller} with {@link Trigger} factories for
 * command-based.
 *
 * @see PS5Controller
 */
class CommandPS5Controller : public frc::PS5Controller {
 public:
  using PS5Controller::PS5Controller;

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
   * Constructs an event instance around the square button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the square button's digital signal
   * attached to the given loop.
   */
  Trigger Square(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the cross button's digital signal
   * attached to the given loop.
   */
  Trigger Cross(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the circle button's digital signal
   * attached to the given loop.
   */
  Trigger Circle(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the triangle button's digital signal
   * attached to the given loop.
   */
  Trigger Triangle(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the L1 button's digital signal
   * attached to the given loop.
   */
  Trigger L1(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the R1 button's digital signal
   * attached to the given loop.
   */
  Trigger R1(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the L2 button's digital signal
   * attached to the given loop.
   */
  Trigger L2(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the R2 button's digital signal
   * attached to the given loop.
   */
  Trigger R2(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the options button's digital signal
   * attached to the given loop.
   */
  Trigger Options(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the L3 button's digital signal
   * attached to the given loop.
   */
  Trigger L3(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the R3 button's digital signal
   * attached to the given loop.
   */
  Trigger R3(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the PS button's digital signal
   * attached to the given loop.
   */
  Trigger PS(frc::EventLoop* loop =
                 CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the touchpad's digital signal
   * attached to the given loop.
   */
  Trigger Touchpad(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;
};
}  // namespace frc2
