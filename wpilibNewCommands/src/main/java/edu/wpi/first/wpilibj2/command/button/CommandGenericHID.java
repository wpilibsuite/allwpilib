// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.math.Pair;
import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import java.util.HashMap;
import java.util.Map;

/**
 * A version of {@link GenericHID} with {@link Trigger} factories for command-based.
 *
 * @see GenericHID
 */
public class CommandGenericHID {
  private final GenericHID m_hid;
  private final Map<EventLoop, Map<Integer, Trigger>> m_buttonCache = new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisLessThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisGreaterThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>>
      m_axisMagnitudeGreaterThanCache = new HashMap<>();
  private final Map<EventLoop, Map<Integer, Trigger>> m_povCache = new HashMap<>();

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
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return button(button, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public Trigger button(int button, EventLoop loop) {
    var cache = m_buttonCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(button, k -> new Trigger(loop, () -> m_hid.getRawButton(k)));
  }

  /**
   * Constructs a Trigger instance based around this angle of the default (index 0) POV on the HID,
   * attached to {@link CommandScheduler#getDefaultButtonLoop() the default command scheduler button
   * loop}.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int angle) {
    return pov(0, angle, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @param loop the event loop instance to attach the event to. Defaults to {@link
   *     CommandScheduler#getDefaultButtonLoop() the default command scheduler button loop}.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int pov, int angle, EventLoop loop) {
    var cache = m_povCache.computeIfAbsent(loop, k -> new HashMap<>());
    // angle can be -1, so use 3600 instead of 360
    return cache.computeIfAbsent(
        pov * 3600 + angle, k -> new Trigger(loop, () -> m_hid.getPOV(pov) == angle));
  }

  /**
   * Constructs a Trigger instance based around the 0 degree angle (up) of the default (index 0) POV
   * on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 0 degree angle of a POV on the HID.
   */
  public Trigger povUp() {
    return pov(0);
  }

  /**
   * Constructs a Trigger instance based around the 45 degree angle (right up) of the default (index
   * 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 45 degree angle of a POV on the HID.
   */
  public Trigger povUpRight() {
    return pov(45);
  }

  /**
   * Constructs a Trigger instance based around the 90 degree angle (right) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 90 degree angle of a POV on the HID.
   */
  public Trigger povRight() {
    return pov(90);
  }

  /**
   * Constructs a Trigger instance based around the 135 degree angle (right down) of the default
   * (index 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the
   * default command scheduler button loop}.
   *
   * @return a Trigger instance based around the 135 degree angle of a POV on the HID.
   */
  public Trigger povDownRight() {
    return pov(135);
  }

  /**
   * Constructs a Trigger instance based around the 180 degree angle (down) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 180 degree angle of a POV on the HID.
   */
  public Trigger povDown() {
    return pov(180);
  }

  /**
   * Constructs a Trigger instance based around the 225 degree angle (down left) of the default
   * (index 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the
   * default command scheduler button loop}.
   *
   * @return a Trigger instance based around the 225 degree angle of a POV on the HID.
   */
  public Trigger povDownLeft() {
    return pov(225);
  }

  /**
   * Constructs a Trigger instance based around the 270 degree angle (left) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 270 degree angle of a POV on the HID.
   */
  public Trigger povLeft() {
    return pov(270);
  }

  /**
   * Constructs a Trigger instance based around the 315 degree angle (left up) of the default (index
   * 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 315 degree angle of a POV on the HID.
   */
  public Trigger povUpLeft() {
    return pov(315);
  }

  /**
   * Constructs a Trigger instance based around the center (not pressed) position of the default
   * (index 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the
   * default command scheduler button loop}.
   *
   * @return a Trigger instance based around the center position of a POV on the HID.
   */
  public Trigger povCenter() {
    return pov(-1);
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link CommandScheduler#getDefaultButtonLoop() the default command scheduler button
   * loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold) {
    return axisLessThan(axis, threshold, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisLessThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold), k -> new Trigger(loop, () -> getRawAxis(axis) < threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link CommandScheduler#getDefaultButtonLoop() the default command scheduler button
   * loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold) {
    return axisGreaterThan(axis, threshold, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold), k -> new Trigger(loop, () -> getRawAxis(axis) > threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis magnitude value is greater than the
   *     provided threshold.
   */
  public Trigger axisMagnitudeGreaterThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisMagnitudeGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold),
        k -> new Trigger(loop, () -> Math.abs(getRawAxis(axis)) > threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is greater than {@code
   * threshold}, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the deadbanded axis value is active (non-zero).
   */
  public Trigger axisMagnitudeGreaterThan(int axis, double threshold) {
    return axisMagnitudeGreaterThan(
        axis, threshold, CommandScheduler.getInstance().getDefaultButtonLoop());
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

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  public void setRumble(GenericHID.RumbleType type, double value) {
    m_hid.setRumble(type, value);
  }

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  public boolean isConnected() {
    return m_hid.isConnected();
  }
}
