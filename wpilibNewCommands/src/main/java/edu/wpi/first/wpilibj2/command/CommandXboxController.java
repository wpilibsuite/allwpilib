// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;

/**
 * Provides JoystickButtons for binding commands to an XboxController's buttons. Additionally offers
 * getters for retrieving axis values.
 */
public class CommandXboxController extends GenericHID {
  // reuses the Button and Axis enums from the original XboxController

  private JoystickButton m_leftBumper;
  private JoystickButton m_rightBumper;
  private JoystickButton m_leftStick;
  private JoystickButton m_rightStick;
  private JoystickButton m_a;
  private JoystickButton m_b;
  private JoystickButton m_x;
  private JoystickButton m_y;
  private JoystickButton m_backButton;
  private JoystickButton m_startButton;

  @SuppressWarnings("checkstyle:MemberName")
  public final CommandControllerPOV pov;

  /**
   * Constructs a CommandXboxController.
   *
   * @param port The Driver Station port to initialize it on.
   */
  public CommandXboxController(final int port) {
    super(port);
    pov = new CommandControllerPOV(this);

    HAL.report(tResourceType.kResourceType_XboxController, port + 1);
  }

  /**
   * Returns the left bumper's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton leftBumper() {
    if (m_leftBumper == null) {
      m_leftBumper = new JoystickButton(this, XboxController.Button.kLeftBumper.value);
    }

    return m_leftBumper;
  }

  /**
   * Returns the right bumper's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton rightBumper() {
    if (m_rightBumper == null) {
      m_rightBumper = new JoystickButton(this, XboxController.Button.kRightBumper.value);
    }

    return m_rightBumper;
  }

  /**
   * Returns the left stick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton leftStick() {
    if (m_leftStick == null) {
      m_leftStick = new JoystickButton(this, XboxController.Button.kLeftStick.value);
    }

    return m_leftStick;
  }

  /**
   * Returns the right stick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton rightStick() {
    if (m_rightStick == null) {
      m_rightStick = new JoystickButton(this, XboxController.Button.kRightStick.value);
    }

    return m_rightStick;
  }

  /**
   * Returns the A button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  @SuppressWarnings("checkstyle:MethodName")
  public JoystickButton a() {
    if (m_a == null) {
      m_a = new JoystickButton(this, XboxController.Button.kA.value);
    }

    return m_a;
  }

  /**
   * Returns the B button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  @SuppressWarnings("checkstyle:MethodName")
  public JoystickButton b() {
    if (m_b == null) {
      m_b = new JoystickButton(this, XboxController.Button.kB.value);
    }

    return m_b;
  }

  /**
   * Returns the X button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  @SuppressWarnings("checkstyle:MethodName")
  public JoystickButton x() {
    if (m_x == null) {
      m_x = new JoystickButton(this, XboxController.Button.kX.value);
    }

    return m_x;
  }

  /**
   * Returns the Y button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  @SuppressWarnings("checkstyle:MethodName")
  public JoystickButton y() {
    if (m_y == null) {
      m_y = new JoystickButton(this, XboxController.Button.kY.value);
    }

    return m_y;
  }

  /**
   * Returns the back button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton back() {
    if (m_backButton == null) {
      m_backButton = new JoystickButton(this, XboxController.Button.kBack.value);
    }

    return m_backButton;
  }

  /**
   * Returns the start button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton start() {
    if (m_startButton == null) {
      m_startButton = new JoystickButton(this, XboxController.Button.kStart.value);
    }

    return m_startButton;
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return getRawAxis(XboxController.Axis.kLeftX.value);
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getRawAxis(XboxController.Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getRawAxis(XboxController.Axis.kLeftY.value);
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getRawAxis(XboxController.Axis.kRightY.value);
  }

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return getRawAxis(XboxController.Axis.kLeftTrigger.value);
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return getRawAxis(XboxController.Axis.kRightTrigger.value);
  }
}
