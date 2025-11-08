// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

/**
 * A version of {@link Joystick} with {@link Trigger} factories for command-based.
 *
 * @see Joystick
 */
public class CommandJoystick extends CommandGenericHID {
  private final Joystick m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandJoystick(int port) {
    super(port);
    m_hid = new Joystick(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public Joystick getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @return an event instance representing the trigger button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #trigger(EventLoop)
   */
  public Trigger trigger() {
    return trigger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the trigger button's digital signal attached to the
   *     given loop.
   */
  public Trigger trigger(EventLoop loop) {
    return button(Joystick.ButtonType.kTrigger.value, loop);
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @return an event instance representing the top button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #top(EventLoop)
   */
  public Trigger top() {
    return top(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the top button's digital signal attached to the given
   *     loop.
   */
  public Trigger top(EventLoop loop) {
    return button(Joystick.ButtonType.kTop.value, loop);
  }

  /**
   * Set the channel associated with the X axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setXChannel(int channel) {
    m_hid.setXChannel(channel);
  }

  /**
   * Set the channel associated with the Y axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setYChannel(int channel) {
    m_hid.setYChannel(channel);
  }

  /**
   * Set the channel associated with the Z axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setZChannel(int channel) {
    m_hid.setZChannel(channel);
  }

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setThrottleChannel(int channel) {
    m_hid.setThrottleChannel(channel);
  }

  /**
   * Set the channel associated with the twist axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setTwistChannel(int channel) {
    m_hid.setTwistChannel(channel);
  }

  /**
   * Get the channel currently associated with the X axis.
   *
   * @return The channel for the axis.
   */
  public int getXChannel() {
    return m_hid.getXChannel();
  }

  /**
   * Get the channel currently associated with the Y axis.
   *
   * @return The channel for the axis.
   */
  public int getYChannel() {
    return m_hid.getYChannel();
  }

  /**
   * Get the channel currently associated with the Z axis.
   *
   * @return The channel for the axis.
   */
  public int getZChannel() {
    return m_hid.getZChannel();
  }

  /**
   * Get the channel currently associated with the twist axis.
   *
   * @return The channel for the axis.
   */
  public int getTwistChannel() {
    return m_hid.getTwistChannel();
  }

  /**
   * Get the channel currently associated with the throttle axis.
   *
   * @return The channel for the axis.
   */
  public int getThrottleChannel() {
    return m_hid.getThrottleChannel();
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
    return m_hid.getX();
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
    return m_hid.getY();
  }

  /**
   * Get the z position of the HID.
   *
   * @return the z position
   */
  public double getZ() {
    return m_hid.getZ();
  }

  /**
   * Get the twist value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Twist value of the joystick.
   */
  public double getTwist() {
    return m_hid.getTwist();
  }

  /**
   * Get the throttle value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Throttle value of the joystick.
   */
  public double getThrottle() {
    return m_hid.getThrottle();
  }

  /**
   * Get the magnitude of the vector formed by the joystick's current position relative to its
   * origin.
   *
   * @return The magnitude of the direction vector
   */
  public double getMagnitude() {
    return m_hid.getMagnitude();
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
    return m_hid.getDirectionRadians();
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in degrees. 0 is forward
   * and clockwise is positive. (Straight right is 90.)
   *
   * @return The direction of the vector in degrees
   */
  public double getDirectionDegrees() {
    return m_hid.getDirectionDegrees();
  }
}
