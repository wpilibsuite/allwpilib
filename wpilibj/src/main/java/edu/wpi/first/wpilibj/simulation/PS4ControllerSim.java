// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.PS4Controller;

/** Class to control a simulated PS4 controller. */
public class PS4ControllerSim extends GenericHIDSim {
  /**
   * Constructs from a PS4Controller object.
   *
   * @param joystick controller to simulate
   */
  public PS4ControllerSim(PS4Controller joystick) {
    super(joystick);
    setAxisCount(6);
    setButtonCount(14);
    setPOVCount(1);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  public PS4ControllerSim(int port) {
    super(port);
    setAxisCount(6);
    setButtonCount(14);
    setPOVCount(1);
  }

  /**
   * Change the X axis value of the controller's left stick.
   *
   * @param value the new value
   */
  public void setLeftX(double value) {
    setRawAxis(PS4Controller.Axis.kLeftX.value, value);
  }

  /**
   * Change the X axis value of the controller's right stick.
   *
   * @param value the new value
   */
  public void setRightX(double value) {
    setRawAxis(PS4Controller.Axis.kRightX.value, value);
  }

  /**
   * Change the Y axis value of the controller's left stick.
   *
   * @param value the new value
   */
  public void setLeftY(double value) {
    setRawAxis(PS4Controller.Axis.kLeftY.value, value);
  }

  /**
   * Change the Y axis value of the controller's right stick.
   *
   * @param value the new value
   */
  public void setRightY(double value) {
    setRawAxis(PS4Controller.Axis.kRightY.value, value);
  }

  /**
   * Change the L2 axis axis value of the controller.
   *
   * @param value the new value
   */
  public void setL2Axis(double value) {
    setRawAxis(PS4Controller.Axis.kL2.value, value);
  }

  /**
   * Change the R2 axis value of the controller.
   *
   * @param value the new value
   */
  public void setR2Axis(double value) {
    setRawAxis(PS4Controller.Axis.kR2.value, value);
  }

  /**
   * Change the value of the Square button on the controller.
   *
   * @param value the new value
   */
  public void setSquareButton(boolean value) {
    setRawButton(PS4Controller.Button.kSquare.value, value);
  }

  /**
   * Change the value of the Cross button on the controller.
   *
   * @param value the new value
   */
  public void setCrossButton(boolean value) {
    setRawButton(PS4Controller.Button.kCross.value, value);
  }

  /**
   * Change the value of the Circle button on the controller.
   *
   * @param value the new value
   */
  public void setCircleButton(boolean value) {
    setRawButton(PS4Controller.Button.kCircle.value, value);
  }

  /**
   * Change the value of the Triangle button on the controller.
   *
   * @param value the new value
   */
  public void setTriangleButton(boolean value) {
    setRawButton(PS4Controller.Button.kTriangle.value, value);
  }

  /**
   * Change the value of the L1 button on the controller.
   *
   * @param value the new value
   */
  public void setL1Button(boolean value) {
    setRawButton(PS4Controller.Button.kL1.value, value);
  }

  /**
   * Change the value of the R1 button on the controller.
   *
   * @param value the new value
   */
  public void setR1Button(boolean value) {
    setRawButton(PS4Controller.Button.kR1.value, value);
  }

  /**
   * Change the value of the L2 button on the controller.
   *
   * @param value the new value
   */
  public void setL2Button(boolean value) {
    setRawButton(PS4Controller.Button.kL2.value, value);
  }

  /**
   * Change the value of the R2 button on the controller.
   *
   * @param value the new value
   */
  public void setR2Button(boolean value) {
    setRawButton(PS4Controller.Button.kR2.value, value);
  }

  /**
   * Change the value of the Share button on the controller.
   *
   * @param value the new value
   */
  public void setShareButton(boolean value) {
    setRawButton(PS4Controller.Button.kShare.value, value);
  }

  /**
   * Change the value of the Options button on the controller.
   *
   * @param value the new value
   */
  public void setOptionsButton(boolean value) {
    setRawButton(PS4Controller.Button.kOptions.value, value);
  }

  /**
   * Change the value of the L3 (left stick) button on the controller.
   *
   * @param value the new value
   */
  public void setL3Button(boolean value) {
    setRawButton(PS4Controller.Button.kL3.value, value);
  }

  /**
   * Change the value of the R3 (right stick) button on the controller.
   *
   * @param value the new value
   */
  public void setR3Button(boolean value) {
    setRawButton(PS4Controller.Button.kR3.value, value);
  }

  /**
   * Change the value of the PS button on the controller.
   *
   * @param value the new value
   */
  public void setPSButton(boolean value) {
    setRawButton(PS4Controller.Button.kPS.value, value);
  }

  /**
   * Change the value of the touchpad button on the controller.
   *
   * @param value the new value
   */
  public void setTouchpad(boolean value) {
    setRawButton(PS4Controller.Button.kTouchpad.value, value);
  }
}
