/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * Handle input from standard Joysticks connected to the Driver Station. This class handles standard
 * input that comes from the Driver Station. Each time a value is requested the most recent value is
 * returned. There is a single class instance for each joystick and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 */
public class Joystick extends JoystickBase {

  static final byte kDefaultXAxis = 0;
  static final byte kDefaultYAxis = 1;
  static final byte kDefaultZAxis = 2;
  static final byte kDefaultTwistAxis = 2;
  static final byte kDefaultThrottleAxis = 3;
  static final int kDefaultTriggerButton = 1;
  static final int kDefaultTopButton = 2;

  /**
   * Represents an analog axis on a joystick.
   */
  public enum AxisType {
    kX(0), kY(1), kZ(2), kTwist(3), kThrottle(4), kNumAxis(5);

    @SuppressWarnings("MemberName")
    public final int value;

    private AxisType(int value) {
      this.value = value;
    }
  }

  /**
   * Represents a digital button on the JoyStick.
   */
  public enum ButtonType {
    kTrigger(0), kTop(1), kNumButton(2);

    @SuppressWarnings("MemberName")
    public final int value;

    private ButtonType(int value) {
      this.value = value;
    }
  }

  private final DriverStation m_ds;
  private final byte[] m_axes;
  private final byte[] m_buttons;
  private int m_outputs;
  private short m_leftRumble;
  private short m_rightRumble;

  /**
   * Construct an instance of a joystick. The joystick index is the USB port on the drivers
   * station.
   *
   * @param port The port on the Driver Station that the joystick is plugged into.
   */
  public Joystick(final int port) {
    this(port, AxisType.kNumAxis.value, ButtonType.kNumButton.value);

    m_axes[AxisType.kX.value] = kDefaultXAxis;
    m_axes[AxisType.kY.value] = kDefaultYAxis;
    m_axes[AxisType.kZ.value] = kDefaultZAxis;
    m_axes[AxisType.kTwist.value] = kDefaultTwistAxis;
    m_axes[AxisType.kThrottle.value] = kDefaultThrottleAxis;

    m_buttons[ButtonType.kTrigger.value] = kDefaultTriggerButton;
    m_buttons[ButtonType.kTop.value] = kDefaultTopButton;

    HAL.report(tResourceType.kResourceType_Joystick, port);
  }

  /**
   * Protected version of the constructor to be called by sub-classes.
   *
   * <p>This constructor allows the subclass to configure the number of constants for axes and
   * buttons.
   *
   * @param port           The port on the Driver Station that the joystick is plugged into.
   * @param numAxisTypes   The number of axis types in the enum.
   * @param numButtonTypes The number of button types in the enum.
   */
  protected Joystick(int port, int numAxisTypes, int numButtonTypes) {
    super(port);

    m_ds = DriverStation.getInstance();
    m_axes = new byte[numAxisTypes];
    m_buttons = new byte[numButtonTypes];
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
    return getRawAxis(m_axes[AxisType.kX.value]);
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
    return getRawAxis(m_axes[AxisType.kY.value]);
  }

  @Override
  public final double getZ(Hand hand) {
    return getRawAxis(m_axes[AxisType.kZ.value]);
  }

  /**
   * Get the twist value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Twist value of the joystick.
   */
  public double getTwist() {
    return getRawAxis(m_axes[AxisType.kTwist.value]);
  }

  /**
   * Get the throttle value of the current joystick. This depends on the mapping of the joystick
   * connected to the current port.
   *
   * @return The Throttle value of the joystick.
   */
  public double getThrottle() {
    return getRawAxis(m_axes[AxisType.kThrottle.value]);
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(final int axis) {
    return m_ds.getStickAxis(getPort(), axis);
  }

  /**
   * For the current joystick, return the axis determined by the argument.
   *
   * <p>This is for cases where the joystick axis is returned programatically, otherwise one of the
   * previous functions would be preferable (for example getX()).
   *
   * @param axis The axis to read.
   * @return The value of the axis.
   */
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
   * For the current joystick, return the number of axis.
   */
  public int getAxisCount() {
    return m_ds.getStickAxisCount(getPort());
  }

  /**
   * Read the state of the trigger on the joystick.
   *
   * <p>Look up which button has been assigned to the trigger and read its state.
   *
   * @param hand This parameter is ignored for the Joystick class and is only here to complete the
   *             GenericHID interface.
   * @return The state of the trigger.
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public boolean getTrigger(Hand hand) {
    return getRawButton(m_buttons[ButtonType.kTrigger.value]);
  }

  /**
   * Read the state of the top button on the joystick.
   *
   * <p>Look up which button has been assigned to the top and read its state.
   *
   * @param hand This parameter is ignored for the Joystick class and is only here to complete the
   *             GenericHID interface.
   * @return The state of the top button.
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public boolean getTop(Hand hand) {
    return getRawButton(m_buttons[ButtonType.kTop.value]);
  }

  @Override
  public int getPOV(int pov) {
    return m_ds.getStickPOV(getPort(), pov);
  }

  @Override
  public int getPOVCount() {
    return m_ds.getStickPOVCount(getPort());
  }

  /**
   * This is not supported for the Joystick. This method is only here to complete the GenericHID
   * interface.
   *
   * @param hand This parameter is ignored for the Joystick class and is only here to complete the
   *             GenericHID interface.
   * @return The state of the bumper (always false)
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public boolean getBumper(Hand hand) {
    return false;
  }

  /**
   * Get the button value (starting at button 1).
   *
   * <p>The appropriate button is returned as a boolean value.
   *
   * @param button The button number to be read (starting at 1).
   * @return The state of the button.
   */
  public boolean getRawButton(final int button) {
    return m_ds.getStickButton(getPort(), (byte) button);
  }

  /**
   * For the current joystick, return the number of buttons.
   */
  public int getButtonCount() {
    return m_ds.getStickButtonCount(getPort());
  }

  /**
   * Get buttons based on an enumerated type.
   *
   * <p>The button type will be looked up in the list of buttons and then read.
   *
   * @param button The type of button to read.
   * @return The state of the button.
   */
  public boolean getButton(ButtonType button) {
    switch (button) {
      case kTrigger:
        return getTrigger();
      case kTop:
        return getTop();
      default:
        return false;
    }
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
   * <p>Uses acos(-1) to represent Pi due to absence of readily accessable Pi constant in C++
   *
   * @return The direction of the vector in degrees
   */
  public double getDirectionDegrees() {
    return Math.toDegrees(getDirectionRadians());
  }

  /**
   * Get the channel currently associated with the specified axis.
   *
   * @param axis The axis to look up the channel for.
   * @return The channel fr the axis.
   */
  public int getAxisChannel(AxisType axis) {
    return m_axes[axis.value];
  }

  /**
   * Set the channel associated with a specified axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  public void setAxisChannel(AxisType axis, int channel) {
    m_axes[axis.value] = (byte) channel;
  }

  /**
   * Get the value of isXbox for the current joystick.
   *
   * @return A boolean that is true if the controller is an xbox controller.
   */
  public boolean getIsXbox() {
    return m_ds.getJoystickIsXbox(getPort());
  }

  /**
   * Get the axis type of a joystick axis.
   *
   * @return the axis type of a joystick axis.
   */
  public int getAxisType(int axis) {
    return m_ds.getJoystickAxisType(getPort(), axis);
  }

  /**
   * Get the type of the HID.
   *
   * @return the type of the HID.
   */
  @Override
  public HIDType getType() {
    return HIDType.values()[m_ds.getJoystickType(getPort())];
  }

  /**
   * Get the name of the HID.
   *
   * @return the name of the HID.
   */
  @Override
  public String getName() {
    return m_ds.getJoystickName(getPort());
  }

  @Override
  public void setOutput(int outputNumber, boolean value) {
    m_outputs = (m_outputs & ~(1 << (outputNumber - 1))) | ((value ? 1 : 0) << (outputNumber - 1));
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }

  @Override
  public void setOutputs(int value) {
    m_outputs = value;
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }

  @Override
  public void setRumble(RumbleType type, double value) {
    if (value < 0) {
      value = 0;
    } else if (value > 1) {
      value = 1;
    }
    if (type == RumbleType.kLeftRumble) {
      m_leftRumble = (short) (value * 65535);
    } else {
      m_rightRumble = (short) (value * 65535);
    }
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }
}
