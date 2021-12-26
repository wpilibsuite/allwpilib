// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.PS4Controller;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;
import edu.wpi.first.wpilibj2.command.button.POVButton;

/**
 * Provides JoystickButtons for binding commands to an PS4Controller's buttons. Additionally offers
 * getters for retrieving axis values.
 */
public class CommandPS4Controller extends GenericHID {
  // reuses the Button and Axis enums from the original PS4Controller

  private JoystickButton m_squareButton;
  private JoystickButton m_crossButton;
  private JoystickButton m_circleButton;
  private JoystickButton m_triangleButton;

  private JoystickButton m_aButton;
  private JoystickButton m_bButton;
  private JoystickButton m_xButton;
  private JoystickButton m_yButton;

  private JoystickButton m_backButton;
  private JoystickButton m_startButton;
  private POVButton m_uButton; // 0 degrees 
  private POVButton m_urButton; // 45 degrees
  private POVButton m_rButton; // 90 degrees
  private POVButton m_drButton; // 135 degrees
  private POVButton m_dButton; // 180 degrees
  private POVButton m_dlButton; // 225 degrees
  private POVButton m_lButton; // 270 degrees
  private POVButton m_ulButton; // 315 degrees

  /**
   * Constructs a CommandPS4Controller.
   *
   * @param port The Driver Station port to initialize it on.
   */
  public CommandPS4Controller(final int port) {
    super(port);

    HAL.report(tResourceType.kResourceType_PS4Controller, port + 1);
  }

  /**
   * Returns the left bumper's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton squareButton() {
    if (m_squareButton == null) {
      m_squareButton = new JoystickButton(this, PS4Controller.Button.kSquare.value);
    }

    return m_squareButton;
  }

  /**
   * Returns the right bumper's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton crossButton() {
    if (m_crossButton == null) {
      m_crossButton = new JoystickButton(this, PS4Controller.Button.kCross.value);
    }

    return m_crossButton;
  }

  /**
   * Returns the left stick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton circleButton() {
    if (m_circleButton == null) {
      m_circleButton = new JoystickButton(this, PS4Controller.Button.kCircle.value);
    }

    return m_circleButton;
  }

  /**
   * Returns the right stick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton triangleButton() {
    if (m_triangleButton == null) {
      m_triangleButton = new JoystickButton(this, PS4Controller.Button.kTriangle.value);
    }

    return m_triangleButton;
  }

  /**
   * Returns the A button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */  
  public JoystickButton aButton() {
    if (m_aButton == null) {
      m_aButton = new JoystickButton(this, PS4Controller.Button.kA.value);
    }

    return m_aButton;
  }

  /**
   * Returns the B button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton bButton() {
    if (m_bButton == null) {
      m_bButton = new JoystickButton(this, PS4Controller.Button.kB.value);
    }

    return m_bButton;
  }

  /**
   * Returns the X button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton xButton() {
    if (m_xButton == null) {
      m_xButton = new JoystickButton(this, PS4Controller.Button.kX.value);
    }

    return m_xButton;
  }

  /**
   * Returns the Y button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton yButton() {
    if (m_yButton == null) {
      m_yButton = new JoystickButton(this, PS4Controller.Button.kY.value);
    }

    return m_yButton;
  }

  /**
   * Returns the back button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton backButton() {
    if (m_backButton == null) {
      m_backButton = new JoystickButton(this, PS4Controller.Button.kBack.value);
    }

    return m_backButton;
  }

  /**
   * Returns the start button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton startButton() {
    if (m_startButton == null) {
      m_startButton = new JoystickButton(this, PS4Controller.Button.kStart.value);
    }

    return m_startButton;
  }

  /**
   * Returns the upper (0 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton uButton() {
    if (m_uButton == null) {
      m_uButton = new POVButton(this, 0);
    }

    return m_uButton;
  }

  /**
   * Returns the upper-right (45 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton urButton() {
    if (m_urButton == null) {
      m_urButton = new POVButton(this, 45);
    }

    return m_urButton;
  }

  /**
   * Returns the right (90 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton rButton() {
    if (m_rButton == null) {
      m_rButton = new POVButton(this, 90);
    }

    return m_rButton;
  }

  /**
   * Returns the downwards-right (135 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton drButton() {
    if (m_drButton == null) {
      m_drButton = new POVButton(this, 135);
    }

    return m_drButton;
  }

  /**
   * Returns the downwards (180 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton dButton() {
    if (m_dButton == null) {
      m_dButton = new POVButton(this, 180);
    }

    return m_dButton;
  }

  /**
   * Returns the downwards-left (225 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton dlButton() {
    if (m_dlButton == null) {
      m_dlButton = new POVButton(this, 225);
    }

    return m_dlButton;
  }

  /**
   * Returns the left (270 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton lButton() {
    if (m_lButton == null) {
      m_lButton = new POVButton(this, 270);
    }

    return m_lButton;
  }

  /**
   * Returns the upwards-left (315 degrees) POVButton object.
   *
   * <p>To get its value, use {@link POVButton#get()}.
   */
  public POVButton ulButton() {
    if (m_ulButton == null) {
      m_ulButton = new POVButton(this, 315);
    }

    return m_ulButton;
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
  public double getLeftTriggerAxis() {
    return getRawAxis(PS4Controller.Axis.kL2.value);
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return getRawAxis(PS4Controller.Axis.kR2.value);
  }
}
