// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/Joystick.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/button/CommandGenericHID.h"

namespace frc2 {
/**
 * A version of {@link frc::Joystick} with {@link Trigger} factories for
 * command-based.
 *
 * @see frc::Joystick
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
  frc::Joystick& GetHID();

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
  units::radian_t GetDirection() const;

 private:
  frc::Joystick m_hid;
};
}  // namespace frc2
