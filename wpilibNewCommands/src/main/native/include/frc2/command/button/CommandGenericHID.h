// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/GenericHID.h>

#include "Trigger.h"
#include "frc2/command/CommandScheduler.h"

namespace frc2 {

/**
 * A version of {@link frc::GenericHID} with {@link Trigger} factories for
 * command-based.
 *
 * @see GenericHID
 */
class CommandGenericHID {
 public:
  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged
   * into.
   */
  explicit CommandGenericHID(int port);

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  frc::GenericHID& GetHID();

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
   * Constructs a Trigger instance based around the 0 degree angle (up) of the
   * default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 0 degree angle of a POV on the
   * HID.
   */
  Trigger POVUp(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 45 degree angle (right up)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 45 degree angle of a POV on the
   * HID.
   */
  Trigger POVUpRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 90 degree angle (right) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 90 degree angle of a POV on the
   * HID.
   */
  Trigger POVRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 135 degree angle (right
   * down) of the default (index 0) POV on the HID.
   *
   * @return a Trigger instance based around the 135 degree angle of a POV on
   * the HID.
   */
  Trigger POVDownRight(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 180 degree angle (down) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 180 degree angle of a POV on
   * the HID.
   */
  Trigger POVDown(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 225 degree angle (down left)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 225 degree angle of a POV on
   * the HID.
   */
  Trigger POVDownLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 270 degree angle (left) of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 270 degree angle of a POV on
   * the HID.
   */
  Trigger POVLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the 315 degree angle (left up)
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the 315 degree angle of a POV on
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

  /**
   * Constructs a Trigger instance that is true when the axis value is less than
   * {@code threshold}, attached to {@link
   * CommandScheduler::GetDefaultButtonLoop() the default command scheduler
   * button loop}.
   * @param axis The axis to read, starting at 0.
   * @param threshold The value below which this trigger should return true.
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance that is true when the axis value is less than
   * the provided threshold.
   */
  Trigger AxisLessThan(
      int axis, double threshold,
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance that is true when the axis value is greater
   * than {@code threshold}, attached to {@link
   * CommandScheduler::GetDefaultButtonLoop() the default command scheduler
   * button loop}.
   * @param axis The axis to read, starting at 0.
   * @param threshold The value below which this trigger should return true.
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance that is true when the axis value is greater than
   * the provided threshold.
   */
  Trigger AxisGreaterThan(
      int axis, double threshold,
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is
   * greater than {@code threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis magnitude value is
   * greater than the provided threshold.
   */
  Trigger AxisMagnitudeGreaterThan(
      int axis, double threshold,
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Set the rumble output for the HID.
   *
   * The DS currently supports 2 rumble values, left rumble and right rumble.
   *
   * @param type  Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  void SetRumble(frc::GenericHID::RumbleType type, double value);

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  bool IsConnected() const;

 private:
  frc::GenericHID m_hid;
};
}  // namespace frc2
