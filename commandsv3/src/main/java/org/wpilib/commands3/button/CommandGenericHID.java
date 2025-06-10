// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.button;

import edu.wpi.first.wpilibj.GenericHID;
import org.wpilib.commands3.Scheduler;
import org.wpilib.commands3.Trigger;

/**
 * A version of {@link GenericHID} with {@link Trigger} factories for command-based.
 *
 * @see GenericHID
 */
public class CommandGenericHID {
  private final GenericHID m_hid;

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandGenericHID(int port) {
    m_hid = new GenericHID(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  public GenericHID getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     Scheduler#getInstance() default scheduler button scheduler}.
   * @see #button(int, Scheduler)
   */
  public Trigger button(int button) {
    return this.button(button, Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param scheduler the scheduler instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given
   *     scheduler.
   */
  public Trigger button(int button, Scheduler scheduler) {
    return new Trigger(scheduler, () -> m_hid.getRawButton(button));
  }

  /**
   * Constructs a Trigger instance based around this angle of the default (index 0) POV on the HID,
   * attached to {@link Scheduler#getInstance() the default command scheduler button scheduler}.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int angle) {
    return pov(0, angle, Scheduler.getInstance());
  }

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @param scheduler the scheduler instance to attach the event to. Defaults to {@link
   *     Scheduler#getInstance() the default command scheduler button scheduler}.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int pov, int angle, Scheduler scheduler) {
    return new Trigger(scheduler, () -> m_hid.getPOV(pov) == angle);
  }

  /**
   * Constructs a Trigger instance based around the 0 degree angle (up) of the default (index 0) POV
   * on the HID, attached to {@link Scheduler#getInstance() the default command scheduler button
   * scheduler}.
   *
   * @return a Trigger instance based around the 0 degree angle of a POV on the HID.
   */
  public Trigger povUp() {
    return pov(0);
  }

  /**
   * Constructs a Trigger instance based around the 45 degree angle (right up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getInstance() the default command scheduler
   * button scheduler}.
   *
   * @return a Trigger instance based around the 45 degree angle of a POV on the HID.
   */
  public Trigger povUpRight() {
    return pov(45);
  }

  /**
   * Constructs a Trigger instance based around the 90 degree angle (right) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getInstance() the default command scheduler button
   * scheduler}.
   *
   * @return a Trigger instance based around the 90 degree angle of a POV on the HID.
   */
  public Trigger povRight() {
    return pov(90);
  }

  /**
   * Constructs a Trigger instance based around the 135 degree angle (right down) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getInstance() the default command
   * scheduler button scheduler}.
   *
   * @return a Trigger instance based around the 135 degree angle of a POV on the HID.
   */
  public Trigger povDownRight() {
    return pov(135);
  }

  /**
   * Constructs a Trigger instance based around the 180 degree angle (down) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getInstance() the default command scheduler button
   * scheduler}.
   *
   * @return a Trigger instance based around the 180 degree angle of a POV on the HID.
   */
  public Trigger povDown() {
    return pov(180);
  }

  /**
   * Constructs a Trigger instance based around the 225 degree angle (down left) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getInstance() the default command
   * scheduler button scheduler}.
   *
   * @return a Trigger instance based around the 225 degree angle of a POV on the HID.
   */
  public Trigger povDownLeft() {
    return pov(225);
  }

  /**
   * Constructs a Trigger instance based around the 270 degree angle (left) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getInstance() the default command scheduler button
   * scheduler}.
   *
   * @return a Trigger instance based around the 270 degree angle of a POV on the HID.
   */
  public Trigger povLeft() {
    return pov(270);
  }

  /**
   * Constructs a Trigger instance based around the 315 degree angle (left up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getInstance() the default command scheduler
   * button scheduler}.
   *
   * @return a Trigger instance based around the 315 degree angle of a POV on the HID.
   */
  public Trigger povUpLeft() {
    return pov(315);
  }

  /**
   * Constructs a Trigger instance based around the center (not pressed) position of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getInstance() the default command
   * scheduler button scheduler}.
   *
   * @return a Trigger instance based around the center position of a POV on the HID.
   */
  public Trigger povCenter() {
    return pov(-1);
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getInstance() the default command scheduler button scheduler}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold) {
    return axisLessThan(axis, threshold, Scheduler.getInstance());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to the given scheduler.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @param scheduler the scheduler instance to attach the trigger to
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold, Scheduler scheduler) {
    return new Trigger(
        scheduler, m_hid.axisLessThan(axis, threshold, scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getInstance() the default command scheduler button scheduler}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold) {
    return axisGreaterThan(axis, threshold, Scheduler.getInstance());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is greater than {@code
   * threshold}, attached to the given scheduler.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param scheduler the scheduler instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold, Scheduler scheduler) {
    return new Trigger(
        scheduler, m_hid.axisGreaterThan(axis, threshold, scheduler.getDefaultEventLoop()));
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(int axis) {
    return m_hid.getRawAxis(axis);
  }
}
