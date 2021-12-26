// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.PS4Controller;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;

/**
 * Provides JoystickButtons for binding commands to an PS4Controller's buttons. Additionally offers
 * getters for retrieving axis values.
 */
public class CommandPS4Controller extends GenericHID {
  // reuses the Button and Axis enums from the original PS4Controller

  private JoystickButton m_square;
  private JoystickButton m_cross;
  private JoystickButton m_circle;
  private JoystickButton m_triangle;
  private JoystickButton m_l1;
  private JoystickButton m_r1;
  private JoystickButton m_l2;
  private JoystickButton m_r2;
  private JoystickButton m_share;
  private JoystickButton m_options;
  private JoystickButton m_l3;
  private JoystickButton m_r3;
  private JoystickButton m_ps;
  private JoystickButton m_touchpad;

  @SuppressWarnings("checkstyle:MemberName")
  public final CommandControllerPOV pov;

  /**
   * Constructs a CommandPS4Controller.
   *
   * @param port The Driver Station port to initialize it on.
   */
  public CommandPS4Controller(final int port) {
    super(port);
    pov = new CommandControllerPOV(this);

    HAL.report(tResourceType.kResourceType_PS4Controller, port + 1);
  }

  /**
   * Returns the square button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton square() {
    if (m_square == null) {
      m_square = new JoystickButton(this, PS4Controller.Button.kSquare.value);
    }

    return m_square;
  }

  /**
   * Returns the cross button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton cross() {
    if (m_cross == null) {
      m_cross = new JoystickButton(this, PS4Controller.Button.kCross.value);
    }

    return m_cross;
  }

  /**
   * Returns the circle button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton circle() {
    if (m_circle == null) {
      m_circle = new JoystickButton(this, PS4Controller.Button.kCircle.value);
    }

    return m_circle;
  }

  /**
   * Returns the triangle button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton triangle() {
    if (m_triangle == null) {
      m_triangle = new JoystickButton(this, PS4Controller.Button.kTriangle.value);
    }

    return m_triangle;
  }

  /**
   * Returns the left shoulder's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l1() {
    if (m_l1 == null) {
      m_l1 = new JoystickButton(this, PS4Controller.Button.kL1.value);
    }

    return m_l1;
  }

  /**
   * Returns the right shoulder's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r1() {
    if (m_r1 == null) {
      m_r1 = new JoystickButton(this, PS4Controller.Button.kR1.value);
    }

    return m_r1;
  }

  /**
   * Returns the left trigger's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l2() {
    if (m_l2 == null) {
      m_l2 = new JoystickButton(this, PS4Controller.Button.kL2.value);
    }

    return m_l2;
  }

  /**
   * Returns the right trigger's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r2() {
    if (m_r2 == null) {
      m_r2 = new JoystickButton(this, PS4Controller.Button.kR2.value);
    }

    return m_r2;
  }

  /**
   * Returns the share button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton share() {
    if (m_share == null) {
      m_share = new JoystickButton(this, PS4Controller.Button.kShare.value);
    }

    return m_share;
  }

  /**
   * Returns the options button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton options() {
    if (m_options == null) {
      m_options = new JoystickButton(this, PS4Controller.Button.kOptions.value);
    }

    return m_options;
  }

  /**
   * Returns the left joystick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l3() {
    if (m_l3 == null) {
      m_l3 = new JoystickButton(this, PS4Controller.Button.kL3.value);
    }

    return m_l3;
  }

  /**
   * Returns the right joystick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r3() {
    if (m_r3 == null) {
      m_r3 = new JoystickButton(this, PS4Controller.Button.kR3.value);
    }

    return m_r3;
  }

  /**
   * Returns the PS button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton ps() {
    if (m_ps == null) {
      m_ps = new JoystickButton(this, PS4Controller.Button.kPS.value);
    }

    return m_ps;
  }

  /**
   * Returns the touchpad's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton touchpad() {
    if (m_touchpad == null) {
      m_touchpad = new JoystickButton(this, PS4Controller.Button.kTouchpad.value);
    }

    return m_touchpad;
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return getRawAxis(PS4Controller.Axis.kLeftX.value);
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getRawAxis(PS4Controller.Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getRawAxis(PS4Controller.Axis.kLeftY.value);
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getRawAxis(PS4Controller.Axis.kRightY.value);
  }

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getL2Axis() {
    return getRawAxis(PS4Controller.Axis.kL2.value);
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getR2Axis() {
    return getRawAxis(PS4Controller.Axis.kR2.value);
  }
}
