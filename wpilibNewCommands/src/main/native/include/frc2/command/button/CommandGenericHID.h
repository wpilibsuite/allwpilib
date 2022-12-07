// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/GenericHID.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {
/**
 * A subclass of {@link GenericHID} with {@link Trigger} factories for
 * command-based.
 *
 * @see GenericHID
 */
class CommandGenericHID : public frc::GenericHID {
 public:
  using GenericHID::GenericHID;

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
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise
   * (eg right is 90, upper-left is 315).
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  Trigger POV(int angle,
              frc::EventLoop* loop =
                  CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise
   * (eg right is 90, upper-left is 315).
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @param pov   index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  Trigger POV(int pov, int angle,
              frc::EventLoop* loop =
                  CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 0-degree angle (up) of the
   * default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 0-degree angle of a POV on the
   * HID.
   */
  Trigger POVUp(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 45-degree angle (right up)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 45-degree angle of a POV on the
   * HID.
   */
  Trigger POVUpRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 90-degree angle (right) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 90-degree angle of a POV on the
   * HID.
   */
  Trigger POVRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 135-degree angle (right
   * down) of the default (index 0) POV on the HID.
   *
   * @return a Trigger instance based around the 135-degree angle of a POV on
   * the HID.
   */
  Trigger POVDownRight(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 180-degree angle (down) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 180-degree angle of a POV on
   * the HID.
   */
  Trigger POVDown(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 225-degree angle (down left)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 225-degree angle of a POV on
   * the HID.
   */
  Trigger POVDownLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 270-degree angle (left) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 270-degree angle of a POV on
   * the HID.
   */
  Trigger POVLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 315-degree angle (left up)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 315-degree angle of a POV on
   * the HID.
   */
  Trigger POVUpLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the center (not pressed)
   * position of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the center position of a POV on the
   * HID.
   */
  Trigger POVCenter(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                               .GetDefaultButtonLoop()) const;
};
}  // namespace frc2
