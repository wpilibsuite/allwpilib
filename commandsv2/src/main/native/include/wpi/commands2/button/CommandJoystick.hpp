// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/button/CommandGenericHID.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/driverstation/Joystick.hpp"

namespace wpi::cmd {
/**
 * A version of {@link wpi::Joystick} with {@link Trigger} factories for
 * command-based.
 *
 * @see wpi::Joystick
 */
class CommandJoystick : public CommandGenericHID {
 public:
  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is
   * plugged into.
   */
  explicit CommandJoystick(int port);

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  wpi::Joystick& GetHID();

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the trigger button's digital signal
   * attached to the given loop.
   */
  class Trigger Trigger(
      wpi::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the top button's digital signal
   * attached to the given loop.
   */
  class Trigger Top(wpi::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;
  /**
   * Get the magnitude of the vector formed by the joystick's
   * current position relative to its origin.
   *
   * @return The magnitude of the direction vector
   */
  double GetMagnitude() const;

  /**
   * Get the direction of the vector formed by the joystick and its origin. 0 is
   * forward and clockwise is positive. (Straight right is π/2 radians or 90
   * degrees.)
   *
   * @return The direction of the vector.
   */
  wpi::units::radian_t GetDirection() const;

 private:
  wpi::Joystick m_hid;
};
}  // namespace wpi::cmd
