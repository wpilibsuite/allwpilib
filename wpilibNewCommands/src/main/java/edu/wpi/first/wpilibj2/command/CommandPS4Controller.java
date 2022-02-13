// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj2.command.button.JoystickButton;

import static edu.wpi.first.wpilibj.PS4Controller.Button;

import java.util.EnumMap;

import static edu.wpi.first.wpilibj.PS4Controller.Axis;

/**
 * Provides JoystickButtons for binding commands to an PS4Controller's buttons. Additionally offers
 * getters for retrieving axis values.
 */
public class CommandPS4Controller extends GenericHID {
  // reuses the Button and Axis enums from the original PS4Controller

  private final EnumMap<Button, JoystickButton> m_buttons = new EnumMap<>(Button.class);

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
   * Builds a {@link JoystickButton} for this controller from the provided {@link Button}.
   * @param button the Button to build for
   * @return Built JoystickButton
   */
  private JoystickButton build(Button button) {
		return new JoystickButton(this, button.value);
	}

  /**
   * Returns the square button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton square() {
    return m_buttons.computeIfAbsent(Button.kSquare, this::build);
  }

  /**
   * Returns the cross button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton cross() {
    return m_buttons.computeIfAbsent(Button.kCross, this::build);
  }

  /**
   * Returns the circle button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton circle() {
    return m_buttons.computeIfAbsent(Button.kCircle, this::build);
  }

  /**
   * Returns the triangle button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton triangle() {
    return m_buttons.computeIfAbsent(Button.kTriangle, this::build);
  }

  /**
   * Returns the left shoulder's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l1() {
    return m_buttons.computeIfAbsent(Button.kL1, this::build);
  }

  /**
   * Returns the right shoulder's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r1() {
    return m_buttons.computeIfAbsent(Button.kR1, this::build);
  }

  /**
   * Returns the left trigger's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l2() {
    return m_buttons.computeIfAbsent(Button.kL2, this::build);
  }

  /**
   * Returns the right trigger's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r2() {
    return m_buttons.computeIfAbsent(Button.kR2, this::build);
  }

  /**
   * Returns the share button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton share() {
    return m_buttons.computeIfAbsent(Button.kShare, this::build);
  }

  /**
   * Returns the options button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton options() {
    return m_buttons.computeIfAbsent(Button.kOptions, this::build);
  }

  /**
   * Returns the left joystick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton l3() {
    return m_buttons.computeIfAbsent(Button.kL3, this::build);
  }

  /**
   * Returns the right joystick's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton r3() {
    return m_buttons.computeIfAbsent(Button.kR3, this::build);
  }

  /**
   * Returns the PS button's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton ps() {
    return m_buttons.computeIfAbsent(Button.kPS, this::build);
  }

  /**
   * Returns the touchpad's JoystickButton object.
   *
   * <p>To get its value, use {@link JoystickButton#get()}.
   */
  public JoystickButton touchpad() {
    return m_buttons.computeIfAbsent(Button.kTouchpad, this::build);
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return getRawAxis(Axis.kLeftX.value);
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getRawAxis(Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getRawAxis(Axis.kLeftY.value);
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getRawAxis(Axis.kRightY.value);
  }

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getL2Axis() {
    return getRawAxis(Axis.kL2.value);
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getR2Axis() {
    return getRawAxis(Axis.kR2.value);
  }
}
