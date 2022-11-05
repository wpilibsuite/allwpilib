// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.XboxController;

/** Class to control a simulated Xbox 360 or Xbox One controller. */
public class XboxControllerSim extends GenericHIDSim {
  /**
   * Constructs from a XboxController object.
   *
   * @param joystick controller to simulate
   */
  public XboxControllerSim(XboxController joystick) {
    super(joystick);
    setAxisCount(6);
    setButtonCount(10);
    setPOVCount(1);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  public XboxControllerSim(int port) {
    super(port);
    setAxisCount(6);
    setButtonCount(10);
    setPOVCount(1);
  }

  /**
   * Change the left X value of the joystick.
   *
   * @param value the new value
   */
  public void setLeftX(double value) {
    setRawAxis(XboxController.Axis.kLeftX.value, value);
  }

  /**
   * Change the right X value of the joystick.
   *
   * @param value the new value
   */
  public void setRightX(double value) {
    setRawAxis(XboxController.Axis.kRightX.value, value);
  }

  /**
   * Change the left Y value of the joystick.
   *
   * @param value the new value
   */
  public void setLeftY(double value) {
    setRawAxis(XboxController.Axis.kLeftY.value, value);
  }

  /**
   * Change the right Y value of the joystick.
   *
   * @param value the new value
   */
  public void setRightY(double value) {
    setRawAxis(XboxController.Axis.kRightY.value, value);
  }

  /**
   * Change the value of the left trigger axis on the joystick.
   *
   * @param value the new value
   */
  public void setLeftTriggerAxis(double value) {
    setRawAxis(XboxController.Axis.kLeftTrigger.value, value);
  }

  /**
   * Change the value of the right trigger axis on the joystick.
   *
   * @param value the new value
   */
  public void setRightTriggerAxis(double value) {
    setRawAxis(XboxController.Axis.kRightTrigger.value, value);
  }

  /**
   * Change the value of the left bumper on the joystick.
   *
   * @param state the new value
   */
  public void setLeftBumper(boolean state) {
    setRawButton(XboxController.Button.kLeftBumper.value, state);
  }

  /**
   * Change the value of the right bumper on the joystick.
   *
   * @param state the new value
   */
  public void setRightBumper(boolean state) {
    setRawButton(XboxController.Button.kRightBumper.value, state);
  }

  /**
   * Change the value of the left stick button on the joystick.
   *
   * @param state the new value
   */
  public void setLeftStickButton(boolean state) {
    setRawButton(XboxController.Button.kLeftStick.value, state);
  }

  /**
   * Change the value of the right stick button on the joystick.
   *
   * @param state the new value
   */
  public void setRightStickButton(boolean state) {
    setRawButton(XboxController.Button.kRightStick.value, state);
  }

  /**
   * Change the value of the A button.
   *
   * @param state the new value
   */
  public void setAButton(boolean state) {
    setRawButton(XboxController.Button.kA.value, state);
  }

  /**
   * Change the value of the B button.
   *
   * @param state the new value
   */
  public void setBButton(boolean state) {
    setRawButton(XboxController.Button.kB.value, state);
  }

  /**
   * Change the value of the X button.
   *
   * @param state the new value
   */
  public void setXButton(boolean state) {
    setRawButton(XboxController.Button.kX.value, state);
  }

  /**
   * Change the value of the Y button.
   *
   * @param state the new value
   */
  public void setYButton(boolean state) {
    setRawButton(XboxController.Button.kY.value, state);
  }

  /**
   * Change the value of the Back button.
   *
   * @param state the new value
   */
  public void setBackButton(boolean state) {
    setRawButton(XboxController.Button.kBack.value, state);
  }

  /**
   * Change the value of the Start button.
   *
   * @param state the new value
   */
  public void setStartButton(boolean state) {
    setRawButton(XboxController.Button.kStart.value, state);
  }
}
