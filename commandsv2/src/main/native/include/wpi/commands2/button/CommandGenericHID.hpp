// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/GenericHID.hpp"

#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/commands2/CommandScheduler.hpp"

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
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @param angle POV angle.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  Trigger POV(frc::DriverStation::POVDirection angle,
              frc::EventLoop* loop =
                  CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @param pov   index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  Trigger POV(int pov, frc::DriverStation::POVDirection angle,
              frc::EventLoop* loop =
                  CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the up direction of the
   * default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the up direction of a POV on the
   * HID.
   */
  Trigger POVUp(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                           .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the up right direction
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the up right direction of a POV on
   * the HID.
   */
  Trigger POVUpRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the right direction of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the right direction of a POV on the
   * HID.
   */
  Trigger POVRight(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                              .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the down right direction
   * of the default (index 0) POV on the HID.
   *
   * @return a Trigger instance based around the down right direction of a POV
   * on the HID.
   */
  Trigger POVDownRight(
      frc::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the down direction of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the down direction of a POV on
   * the HID.
   */
  Trigger POVDown(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the down left direction
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the down left direction of a POV on
   * the HID.
   */
  Trigger POVDownLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                                 .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the left direction of
   * the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the left direction of a POV on
   * the HID.
   */
  Trigger POVLeft(frc::EventLoop* loop = CommandScheduler::GetInstance()
                                             .GetDefaultButtonLoop()) const;

  /**
   * Constructs a Trigger instance based around the up left direction
   * of the default (index 0) POV on the HID.
   *
   * @param loop  the event loop instance to attach the event to. Defaults to
   * {@link CommandScheduler::GetDefaultButtonLoop() the default command
   * scheduler button loop}.
   * @return a Trigger instance based around the up left direction of a POV on
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
