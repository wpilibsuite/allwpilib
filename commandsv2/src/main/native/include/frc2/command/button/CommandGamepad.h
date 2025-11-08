// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/Gamepad.h>

#include "frc2/command/CommandScheduler.h"
#include "frc2/command/button/CommandGenericHID.h"
#include "frc2/command/button/Trigger.h"

namespace frc2 {
/**
 * A version of {@link frc::Gamepad} with {@link Trigger} factories for
 * command-based.
 *
 * @see frc::Gamepad
 */
class CommandGamepad : public CommandGenericHID {
 public:
  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is
   * plugged into.
   */
  explicit CommandGamepad(int port);

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  frc::Gamepad& GetHID();

  /**
   * Constructs a Trigger instance around the South Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the South Face button's
   * digital signal attached to the given loop.
   */
  Trigger SouthFace(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the East Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the East Face button's
   * digital signal attached to the given loop.
   */
  Trigger EastFace(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the West Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the West Face button's
   * digital signal attached to the given loop.
   */
  Trigger WestFace(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the North Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the North Face button's
   * digital signal attached to the given loop.
   */
  Trigger NorthFace(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Back button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Back button's
   * digital signal attached to the given loop.
   */
  Trigger Back(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                          .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Guide button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Guide button's
   * digital signal attached to the given loop.
   */
  Trigger Guide(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Start button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Start button's
   * digital signal attached to the given loop.
   */
  Trigger Start(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the left stick button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the left stick button's
   * digital signal attached to the given loop.
   */
  Trigger LeftStick(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the right stick button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the right stick button's
   * digital signal attached to the given loop.
   */
  Trigger RightStick(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the right shoulder button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the right shoulder button's
   * digital signal attached to the given loop.
   */
  Trigger LeftShoulder(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the right shoulder button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the right shoulder button's
   * digital signal attached to the given loop.
   */
  Trigger RightShoulder(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the D-pad up button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the D-pad up button's
   * digital signal attached to the given loop.
   */
  Trigger DpadUp(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the D-pad down button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the D-pad down button's
   * digital signal attached to the given loop.
   */
  Trigger DpadDown(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the D-pad left button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the D-pad left button's
   * digital signal attached to the given loop.
   */
  Trigger DpadLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the D-pad right button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the D-pad right button's
   * digital signal attached to the given loop.
   */
  Trigger DpadRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 1 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc1(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Right Paddle 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Right Paddle 1 button's
   * digital signal attached to the given loop.
   */
  Trigger RightPaddle1(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Left Paddle 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Left Paddle 1 button's
   * digital signal attached to the given loop.
   */
  Trigger LeftPaddle1(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Right Paddle 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Right Paddle 2 button's
   * digital signal attached to the given loop.
   */
  Trigger RightPaddle2(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Left Paddle 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Left Paddle 2 button's
   * digital signal attached to the given loop.
   */
  Trigger LeftPaddle2(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Touchpad button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Touchpad button's
   * digital signal attached to the given loop.
   */
  Trigger Touchpad(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 2 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc2(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 3 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 3 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc3(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 4 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 4 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc4(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 5 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 5 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc5(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance around the Miscellaneous 6 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return a Trigger instance representing the Miscellaneous 6 button's
   * digital signal attached to the given loop.
   */
  Trigger Misc6(frc::EventLoop* loop = CommandScheduler::GetInstance()
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

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  double GetLeftX() const;

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  double GetLeftY() const;

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  double GetRightX() const;

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  double GetRightY() const;

  /**
   * Get the left trigger axis value of the controller. Note that this axis is
   * bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  double GetLeftTriggerAxis() const;

  /**
   * Get the right trigger axis value of the controller. Note that this axis is
   * bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  double GetRightTriggerAxis() const;

 private:
  frc::Gamepad m_hid;
};
}  // namespace frc2
