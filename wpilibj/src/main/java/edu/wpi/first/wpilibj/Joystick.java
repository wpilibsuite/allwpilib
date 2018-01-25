/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * Handle input from standard Joysticks connected to the Driver Station.
 *
 * <p>This class handles standard input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each joystick
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class Joystick extends GenericHID {
  static final byte kDefaultXAxis = 0;
  static final byte kDefaultYAxis = 1;
  static final byte kDefaultZAxis = 2;
  static final byte kDefaultTwistAxis = 2;
  static final byte kDefaultThrottleAxis = 3;

  /**
   * Represents an analog axis on a joystick.
   */
  public enum AxisType {
    kX(0), kY(1), kZ(2), kTwist(3), kThrottle(4);

    @SuppressWarnings("MemberName")
    public final int value;

    AxisType(int value) {
      this.value = value;
    }
  }

  /**
   * Represents a digital button on a joystick.
   */
  public enum ButtonType {
    kTrigger(1), kTop(2);

    @SuppressWarnings("MemberName")
    public final int value;

    ButtonType(int value) {
      this.value = value;
    }
  }

  /**
   * Represents a digital button on a joystick.
   */
  private enum Button {
    kTrigger(1), kTop(2);

    @SuppressWarnings("MemberName")
    public final int value;

    Button(int value) {
      this.value = value;
    }
  }

  /**
   * Represents an analog axis on a joystick.
   */
  private enum Axis {
    kX(0), kY(1), kZ(2), kTwist(3), kThrottle(4), kNumAxes(5);

    @SuppressWarnings("MemberName")
    public final int value;

    Axis(int value) {
      this.value = value;
    }
  }

  private final byte[] m_axes = new byte[Axis.kNumAxes.value];

  /**
   * Construct an instance of a joystick. The joystick index is the USB port on the drivers
   * station.
   *
   * @param port The port on the Driver Station that the joystick is plugged into.
   */
  public Joystick(final int port) {
    super(port);

    m_axes[Axis.kX.value] = kDefaultXAxis;
    m_axes[Axis.kY.value] = kDefaultYAxis;
    m_axes[Axis.kZ.value] = kDefaultZAxis;
    m_axes[Axis.kTwist.value] = kDefaultTwistAxis;
    m_axes[Axis.kThrottle.value] = kDefaultThrottleAxis;

    HAL.report(tResourceType.kResourceType_Joystick, port);
  }

  /**
   * Set the channel associated with the X axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setXChannel(int channel) {
    m_axes[Axis.kX.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the Y axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setYChannel(int channel) {
    m_axes[Axis.kY.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the Z axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setZChannel(int channel) {
    m_axes[Axis.kZ.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setThrottleChannel(int channel) {
    m_axes[Axis.kThrottle.value] = (byte) channel;
  }

  /**
   * Set the channel associated with the twist axis.
   *
   * @param channel The channel to set the axis to.
   */
  public void setTwistChannel(int channel) {
    m_axes[Axis.kTwist.value] = (byte) channel;
  }

  /**
   * Set the channel associated with a specified axis.
   *
   * @deprecated    Use the more specific axis channel setter functions.
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  @Deprecated
  public void setAxisChannel(AxisType axis, int channel) {
    m_axes[axis.value] = (byte) channel;
  }

  /**
   * Get the channel currently associated with the X axis.
   *
   * @return The channel for the axis.
   */
  public int getXChannel() {
    return m_axes[Axis.kX.value];
  }

  /**
   * Get the channel currently associated with the Y axis.
   *
   * @return The channel for the axis.
   */
  public int getYChannel() {
    return m_axes[Axis.kY.value];
  }

  /**
   * Get the channel currently associated with the Z axis.
   *
   * @return The channel for the axis.
   */
  public int getZChannel() {
    return m_axes[Axis.kZ.value];
  }

  /**
   * Get the channel currently associated with the twist axis.
   *
   * @return The channel for the axis.
   */
  public int getTwistChannel() {
    return m_axes[Axis.kTwist.value];
  }

  /**
   * Get the channel currently associated with the throttle axis.
   *
   * @return The channel for the axis.
   */
  public int getThrottleChannel() {
    return m_axes[Axis.kThrottle.value];
  }

  /**
   * Get the channel currently associated with the specified axis.
   *
   * @deprecated Use the more specific axis channel getter functions.
   * @param axis The axis to look up the channel for.
   * @return The channel for the axis.
   */
  @Deprecated
  public int getAxisChannel(AxisType axis) {
    return m_axes[axis.value];
  }

  /**
   * Get the X value of the joystick. This depends on the mapping of the joystick connected to the
   * current port.
   *
   * @param hand Unused
   * @return The X value of the joystick.
   */
  @Override
  public final double getX(Hand hand) {
    return getRawAxis(m_axes[Axis.kX.value]);
  }

  /**
   * Get the Y value of the joystick. This depends on the mapping of the joystick connected to the
   * current port.
   *
   * @param hand Unused
   * @return The Y value of the joystick.
   */
  @Override
  public final double getY(Hand hand) {
    return getRawAxis(m_axes[Axis.kY.value]);
  }

  /**
   * Get the z position of the HID.
   *
   * @return the z position
   */
  public double getZ() {
    return getRawAxis(m_axes[Axis.kZ.value]);
  }

  /**
   * Get the twist value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Twist value of the joystick.
   */
  public double getTwist() {
    return getRawAxis(m_axes[Axis.kTwist.value]);
  }

  /**
   * Get the throttle value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Throttle value of the joystick.
   */
  public double getThrottle() {
    return getRawAxis(m_axes[Axis.kThrottle.value]);
  }

  /**
   * For the current joystick, return the axis determined by the argument.
   *
   * <p>This is for cases where the joystick axis is returned programmatically, otherwise one of the
   * previous functions would be preferable (for example getX()).
   *
   * @deprecated Use the more specific axis getter functions.
   * @param axis The axis to read.
   * @return The value of the axis.
   */
  @Deprecated
  public double getAxis(final AxisType axis) {
    switch (axis) {
      case kX:
        return getX();
      case kY:
        return getY();
      case kZ:
        return getZ();
      case kTwist:
        return getTwist();
      case kThrottle:
        return getThrottle();
      default:
        return 0.0;
    }
  }

  /**
   * Read the state of the trigger on the joystick.
   *
   * @return The state of the trigger.
   */
  public boolean getTrigger() {
    return getRawButton(Button.kTrigger.value);
  }

  /**
   * Whether the trigger was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTriggerPressed() {
    return getRawButtonPressed(Button.kTrigger.value);
  }

  /**
   * Whether the trigger was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTriggerReleased() {
    return getRawButtonReleased(Button.kTrigger.value);
  }

  /**
   * Read the state of the top button on the joystick.
   *
   * @return The state of the top button.
   */
  public boolean getTop() {
    return getRawButton(Button.kTop.value);
  }

  /**
   * Whether the top button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTopPressed() {
    return getRawButtonPressed(Button.kTop.value);
  }

  /**
   * Whether the top button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTopReleased() {
    return getRawButtonReleased(Button.kTop.value);
  }

  /**
   * Get buttons based on an enumerated type.
   *
   * <p>The button type will be looked up in the list of buttons and then read.
   *
   * @deprecated Use Button enum values instead of ButtonType.
   * @param button The type of button to read.
   * @return The state of the button.
   */
  @Deprecated
  public boolean getButton(ButtonType button) {
    return getRawButton(button.value);
  }

  /**
   * Get the magnitude of the direction vector formed by the joystick's current position relative to
   * its origin.
   *
   * @return The magnitude of the direction vector
   */
  public double getMagnitude() {
    return Math.sqrt(Math.pow(getX(), 2) + Math.pow(getY(), 2));
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in radians.
   *
   * @return The direction of the vector in radians
   */
  public double getDirectionRadians() {
    return Math.atan2(getX(), -getY());
  }

  /**
   * Get the direction of the vector formed by the joystick and its origin in degrees.
   *
   * @return The direction of the vector in degrees
   */
  public double getDirectionDegrees() {
    return Math.toDegrees(getDirectionRadians());
  }
}
