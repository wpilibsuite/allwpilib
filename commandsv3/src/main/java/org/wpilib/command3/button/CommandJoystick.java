// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.driverstation.Joystick;
import org.wpilib.driverstation.POVDirection;
import org.wpilib.event.EventLoop;

/**
 * A version of {@link Joystick} with {@link Trigger} factories for command-based.
 *
 * <p>This class is provided by the Commands v3 VendorDep
 *
 * @see Joystick
 */
public class CommandJoystick {
  private final CommandGenericHID m_hid;
  private final Joystick m_joystick;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandJoystick(int port) {
    this(Scheduler.getDefault(), port);
  }

  /**
   * Construct an instance of a controller.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandJoystick(Scheduler scheduler, int port) {
    m_hid = CommandGenericHID.getCommandGenericHID(scheduler, port);
    m_joystick = new Joystick(DriverStation.getGenericHID(port));
  }

  /**
   * Get the underlying CommandGenericHID object.
   *
   * @return the wrapped CommandGenericHID object
   */
  public CommandGenericHID getHID() {
    return m_hid;
  }

  /**
   * Get the underlying Joystick object.
   *
   * @return the wrapped Joystick object
   */
  public Joystick getJoystick() {
    return m_joystick;
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @return an event instance representing the trigger button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #trigger(EventLoop)
   */
  public Trigger trigger() {
    return trigger(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the trigger button's digital signal attached to the
   *     given loop.
   */
  public Trigger trigger(EventLoop loop) {
    return m_hid.button(Joystick.ButtonType.TRIGGER.value, loop);
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @return an event instance representing the top button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #top(EventLoop)
   */
  public Trigger top() {
    return top(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the top button's digital signal attached to the given
   *     loop.
   */
  public Trigger top(EventLoop loop) {
    return m_hid.button(Joystick.ButtonType.TOP.value, loop);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return m_hid.button(button);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public Trigger button(int button, EventLoop loop) {
    return m_hid.button(button, loop);
  }

  /**
   * Constructs a Trigger instance based around this angle of the default (index 0) POV on the HID,
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param angle POV angle
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(POVDirection angle) {
    return m_hid.pov(angle);
  }

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle
   * @param loop the event loop instance to attach the event to. Defaults to {@link
   *     Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int pov, POVDirection angle, EventLoop loop) {
    return m_hid.pov(pov, angle, loop);
  }

  /**
   * Constructs a Trigger instance based around the 0 degree angle (up) of the default (index 0) POV
   * on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler
   * button loop}.
   *
   * @return a Trigger instance based around the 0 degree angle of a POV on the HID.
   */
  public Trigger povUp() {
    return m_hid.povUp();
  }

  /**
   * Constructs a Trigger instance based around the 45 degree angle (right up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 45 degree angle of a POV on the HID.
   */
  public Trigger povUpRight() {
    return m_hid.povUpRight();
  }

  /**
   * Constructs a Trigger instance based around the 90 degree angle (right) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 90 degree angle of a POV on the HID.
   */
  public Trigger povRight() {
    return m_hid.povRight();
  }

  /**
   * Constructs a Trigger instance based around the 135 degree angle (right down) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 135 degree angle of a POV on the HID.
   */
  public Trigger povDownRight() {
    return m_hid.povDownRight();
  }

  /**
   * Constructs a Trigger instance based around the 180 degree angle (down) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 180 degree angle of a POV on the HID.
   */
  public Trigger povDown() {
    return m_hid.povDown();
  }

  /**
   * Constructs a Trigger instance based around the 225 degree angle (down left) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 225 degree angle of a POV on the HID.
   */
  public Trigger povDownLeft() {
    return m_hid.povDownLeft();
  }

  /**
   * Constructs a Trigger instance based around the 270 degree angle (left) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 270 degree angle of a POV on the HID.
   */
  public Trigger povLeft() {
    return m_hid.povLeft();
  }

  /**
   * Constructs a Trigger instance based around the 315 degree angle (left up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 315 degree angle of a POV on the HID.
   */
  public Trigger povUpLeft() {
    return m_hid.povUpLeft();
  }

  /**
   * Constructs a Trigger instance based around the center (not pressed) position of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the center position of a POV on the HID.
   */
  public Trigger povCenter() {
    return m_hid.povCenter();
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold) {
    return m_hid.axisLessThan(axis, threshold);
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
    return m_hid.axisLessThan(axis, threshold, loop);
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold) {
    return m_hid.axisGreaterThan(axis, threshold);
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
    return m_hid.axisGreaterThan(axis, threshold, loop);
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
    return m_hid.axisMagnitudeGreaterThan(axis, threshold, loop);
  }

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is greater than {@code
   * threshold}, attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler
   * button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the axis magnitude value is greater than the provided threshold.
   */
  public Trigger axisMagnitudeGreaterThan(int axis, double threshold) {
    return m_hid.axisMagnitudeGreaterThan(axis, threshold);
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

  /**
   * Set the channel associated with the X axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setXChannel(int channel) {
    m_joystick.setXChannel(channel);
  }

  /**
   * Set the channel associated with the Y axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setYChannel(int channel) {
    m_joystick.setYChannel(channel);
  }

  /**
   * Set the channel associated with the Z axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setZChannel(int channel) {
    m_joystick.setZChannel(channel);
  }

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setThrottleChannel(int channel) {
    m_joystick.setThrottleChannel(channel);
  }

  /**
   * Set the channel associated with the twist axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setTwistChannel(int channel) {
    m_joystick.setTwistChannel(channel);
  }

  /**
   * Get the channel currently associated with the X axis.
   *
   * @return The channel for the axis.
   */
  public int getXChannel() {
    return m_joystick.getXChannel();
  }

  /**
   * Get the channel currently associated with the Y axis.
   *
   * @return The channel for the axis.
   */
  public int getYChannel() {
    return m_joystick.getYChannel();
  }

  /**
   * Get the channel currently associated with the Z axis.
   *
   * @return The channel for the axis.
   */
  public int getZChannel() {
    return m_joystick.getZChannel();
  }

  /**
   * Get the channel currently associated with the twist axis.
   *
   * @return The channel for the axis.
   */
  public int getTwistChannel() {
    return m_joystick.getTwistChannel();
  }

  /**
   * Get the channel currently associated with the throttle axis.
   *
   * @return The channel for the axis.
   */
  public int getThrottleChannel() {
    return m_joystick.getThrottleChannel();
  }

  /**
   * Get the x position of the HID.
   *
   * <p>This depends on the mapping of the joystick connected to the current port. On most
   * joysticks, positive is to the right.
   *
   * @return the x position
   */
  public double getX() {
    return m_joystick.getX();
  }

  /**
   * Get the y position of the HID.
   *
   * <p>This depends on the mapping of the joystick connected to the current port. On most
   * joysticks, positive is to the back.
   *
   * @return the y position
   */
  public double getY() {
    return m_joystick.getY();
  }

  /**
   * Get the z position of the HID.
   *
   * @return the z position
   */
  public double getZ() {
    return m_joystick.getZ();
  }

  /**
   * Get the twist value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Twist value of the joystick.
   */
  public double getTwist() {
    return m_joystick.getTwist();
  }

  /**
   * Get the throttle value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Throttle value of the joystick.
   */
  public double getThrottle() {
    return m_joystick.getThrottle();
  }

  /**
   * Get the magnitude of the vector formed by the joystick's current position relative to its
   * origin.
   *
   * @return The magnitude of the direction vector
   */
  public double getMagnitude() {
    return m_joystick.getMagnitude();
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in radians. 0 is forward
   * and clockwise is positive. (Straight right is π/2.)
   *
   * @return The direction of the vector in radians
   */
  public double getDirectionRadians() {
    // https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
    // A positive rotation around the X axis moves the joystick right, and a
    // positive rotation around the Y axis moves the joystick backward. When
    // treating them as translations, 0 radians is measured from the right
    // direction, and angle increases clockwise.
    //
    // It's rotated 90 degrees CCW (y is negated and the arguments are reversed)
    // so that 0 radians is forward.
    return m_joystick.getDirectionRadians();
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in degrees. 0 is forward
   * and clockwise is positive. (Straight right is 90.)
   *
   * @return The direction of the vector in degrees
   */
  public double getDirectionDegrees() {
    return m_joystick.getDirectionDegrees();
  }
}
