// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * Handle input from Flight Joysticks connected to the Driver Station.
 *
 * <p>This class handles standard input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each joystick
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class Joystick extends GenericHID {
  /** Default X axis channel. */
  public static final byte kDefaultXChannel = 0;

  /** Default Y axis channel. */
  public static final byte kDefaultYChannel = 1;

  /** Default Z axis channel. */
  public static final byte kDefaultZChannel = 2;

  /** Default twist axis channel. */
  public static final byte kDefaultTwistChannel = 2;

  /** Default throttle axis channel. */
  public static final byte kDefaultThrottleChannel = 3;

  /** Represents an analog axis on a joystick. */
  public enum AxisType {
    /** X axis. */
    kX(0),
    /** Y axis. */
    kY(1),
    /** Z axis. */
    kZ(2),
    /** Twist axis. */
    kTwist(3),
    /** Throttle axis. */
    kThrottle(4);

    /** AxisType value. */
    public final int value;

    AxisType(int value) {
      this.value = value;
    }
  }

  /** Represents a digital button on a joystick. */
  public enum ButtonType {
    /** kTrigger. */
    kTrigger(1),
    /** kTop. */
    kTop(2);

    /** ButtonType value. */
    public final int value;

    ButtonType(int value) {
      this.value = value;
    }
  }

  private final byte[] m_axes = new byte[AxisType.values().length];

  /**
   * Construct an instance of a joystick.
   *
   * @param port The port index on the Driver Station that the joystick is plugged into.
   */
  public Joystick(final int port) {
    super(port);

    m_axes[AxisType.kX.value] = kDefaultXChannel;
    m_axes[AxisType.kY.value] = kDefaultYChannel;
    m_axes[AxisType.kZ.value] = kDefaultZChannel;
    m_axes[AxisType.kTwist.value] = kDefaultTwistChannel;
    m_axes[AxisType.kThrottle.value] = kDefaultThrottleChannel;

    HAL.report(tResourceType.kResourceType_Joystick, port + 1);
  }

  /**
   * Set the channel associated with the X axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setXChannel(int channel) {
    m_axes[AxisType.kX.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the Y axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setYChannel(int channel) {
    m_axes[AxisType.kY.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the Z axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setZChannel(int channel) {
    m_axes[AxisType.kZ.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setThrottleChannel(int channel) {
    m_axes[AxisType.kThrottle.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the twist axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setTwistChannel(int channel) {
    m_axes[AxisType.kTwist.value] = (byte) channel;
  }

  /**
   * Get the channel currently associated with the X axis.
   *
   * @return The channel for the axis.
   */
  public int getXChannel() {
    return m_axes[AxisType.kX.value];
  }

  /**
   * Get the channel currently associated with the Y axis.
   *
   * @return The channel for the axis.
   */
  public int getYChannel() {
    return m_axes[AxisType.kY.value];
  }

  /**
   * Get the channel currently associated with the Z axis.
   *
   * @return The channel for the axis.
   */
  public int getZChannel() {
    return m_axes[AxisType.kZ.value];
  }

  /**
   * Get the channel currently associated with the twist axis.
   *
   * @return The channel for the axis.
   */
  public int getTwistChannel() {
    return m_axes[AxisType.kTwist.value];
  }

  /**
   * Get the channel currently associated with the throttle axis.
   *
   * @return The channel for the axis.
   */
  public int getThrottleChannel() {
    return m_axes[AxisType.kThrottle.value];
  }

  /**
   * Get the X value of the joystick. This depends on the mapping of the joystick connected to the
   * current port. On most joysticks, positive is to the right.
   *
   * @return The X value of the joystick.
   */
  public final double getX() {
    return getRawAxis(m_axes[AxisType.kX.value]);
  }

  /**
   * Get the Y value of the joystick. This depends on the mapping of the joystick connected to the
   * current port. On most joysticks, positive is to the back.
   *
   * @return The Y value of the joystick.
   */
  public final double getY() {
    return getRawAxis(m_axes[AxisType.kY.value]);
  }

  /**
   * Get the z position of the HID.
   *
   * @return the z position
   */
  public final double getZ() {
    return getRawAxis(m_axes[AxisType.kZ.value]);
  }

  /**
   * Get the twist value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Twist value of the joystick.
   */
  public final double getTwist() {
    return getRawAxis(m_axes[AxisType.kTwist.value]);
  }

  /**
   * Get the throttle value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Throttle value of the joystick.
   */
  public final double getThrottle() {
    return getRawAxis(m_axes[AxisType.kThrottle.value]);
  }

  /**
   * Read the state of the trigger on the joystick.
   *
   * @return The state of the trigger.
   */
  public boolean getTrigger() {
    return getRawButton(ButtonType.kTrigger.value);
  }

  /**
   * Whether the trigger was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTriggerPressed() {
    return getRawButtonPressed(ButtonType.kTrigger.value);
  }

  /**
   * Whether the trigger was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTriggerReleased() {
    return getRawButtonReleased(ButtonType.kTrigger.value);
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the trigger button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent trigger(EventLoop loop) {
    return button(ButtonType.kTrigger.value, loop);
  }

  /**
   * Read the state of the top button on the joystick.
   *
   * @return The state of the top button.
   */
  public boolean getTop() {
    return getRawButton(ButtonType.kTop.value);
  }

  /**
   * Whether the top button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTopPressed() {
    return getRawButtonPressed(ButtonType.kTop.value);
  }

  /**
   * Whether the top button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTopReleased() {
    return getRawButtonReleased(ButtonType.kTop.value);
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the top button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent top(EventLoop loop) {
    return button(ButtonType.kTop.value, loop);
  }

  /**
   * Get the magnitude of the vector formed by the joystick's current position relative to its
   * origin.
   *
   * @return The magnitude of the direction vector
   */
  public double getMagnitude() {
    return Math.hypot(getX(), getY());
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
    return Math.atan2(getX(), -getY());
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in degrees. 0 is forward
   * and clockwise is positive. (Straight right is 90.)
   *
   * @return The direction of the vector in degrees
   */
  public double getDirectionDegrees() {
    return Math.toDegrees(getDirectionRadians());
  }
}
