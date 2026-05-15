// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.driverstation.Gamepad;

/** Class to control a simulated Gamepad controller. */
public class GamepadSim extends GenericHIDSim {
  /**
   * Constructs from a Gamepad object.
   *
   * @param joystick controller to simulate
   */
  @SuppressWarnings("this-escape")
  public GamepadSim(Gamepad joystick) {
    super(joystick);
    setAxesMaximumIndex(6);
    setButtonsMaximumIndex(26);
    setPOVsMaximumIndex(1);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  @SuppressWarnings("this-escape")
  public GamepadSim(int port) {
    super(port);
    setAxesMaximumIndex(6);
    setButtonsMaximumIndex(26);
    setPOVsMaximumIndex(1);
  }

  /**
   * Set the value of a given button.
   *
   * @param button the button to set
   * @param value the new value
   */
  public void setButton(Gamepad.Button button, boolean value) {
    setRawButton(button.value, value);
  }

  /**
   * Set the value of a given axis.
   *
   * @param axis the axis to set
   * @param value the new value
   */
  public void setAxis(Gamepad.Axis axis, double value) {
    setRawAxis(axis.value, value);
  }

  /**
   * Change the left X value of the controller's joystick.
   *
   * @param value the new value
   */
  public void setLeftX(double value) {
    setAxis(Gamepad.Axis.LEFT_X, value);
  }

  /**
   * Change the left Y value of the controller's joystick.
   *
   * @param value the new value
   */
  public void setLeftY(double value) {
    setAxis(Gamepad.Axis.LEFT_Y, value);
  }

  /**
   * Change the right X value of the controller's joystick.
   *
   * @param value the new value
   */
  public void setRightX(double value) {
    setAxis(Gamepad.Axis.RIGHT_X, value);
  }

  /**
   * Change the right Y value of the controller's joystick.
   *
   * @param value the new value
   */
  public void setRightY(double value) {
    setAxis(Gamepad.Axis.RIGHT_Y, value);
  }

  /**
   * Change the value of the left trigger axis on the controller.
   *
   * @param value the new value
   */
  public void setLeftTriggerAxis(double value) {
    setAxis(Gamepad.Axis.LEFT_TRIGGER, value);
  }

  /**
   * Change the value of the right trigger axis on the controller.
   *
   * @param value the new value
   */
  public void setRightTriggerAxis(double value) {
    setAxis(Gamepad.Axis.RIGHT_TRIGGER, value);
  }

  /**
   * Change the value of the South Face button on the controller.
   *
   * @param value the new value
   */
  public void setSouthFaceButton(boolean value) {
    setButton(Gamepad.Button.SOUTH_FACE, value);
  }

  /**
   * Change the value of the East Face button on the controller.
   *
   * @param value the new value
   */
  public void setEastFaceButton(boolean value) {
    setButton(Gamepad.Button.EAST_FACE, value);
  }

  /**
   * Change the value of the West Face button on the controller.
   *
   * @param value the new value
   */
  public void setWestFaceButton(boolean value) {
    setButton(Gamepad.Button.WEST_FACE, value);
  }

  /**
   * Change the value of the North Face button on the controller.
   *
   * @param value the new value
   */
  public void setNorthFaceButton(boolean value) {
    setButton(Gamepad.Button.NORTH_FACE, value);
  }

  /**
   * Change the value of the Back button on the controller.
   *
   * @param value the new value
   */
  public void setBackButton(boolean value) {
    setButton(Gamepad.Button.BACK, value);
  }

  /**
   * Change the value of the Guide button on the controller.
   *
   * @param value the new value
   */
  public void setGuideButton(boolean value) {
    setButton(Gamepad.Button.GUIDE, value);
  }

  /**
   * Change the value of the Start button on the controller.
   *
   * @param value the new value
   */
  public void setStartButton(boolean value) {
    setButton(Gamepad.Button.START, value);
  }

  /**
   * Change the value of the left stick button on the controller.
   *
   * @param value the new value
   */
  public void setLeftStickButton(boolean value) {
    setButton(Gamepad.Button.LEFT_STICK, value);
  }

  /**
   * Change the value of the right stick button on the controller.
   *
   * @param value the new value
   */
  public void setRightStickButton(boolean value) {
    setButton(Gamepad.Button.RIGHT_STICK, value);
  }

  /**
   * Change the value of the right bumper button on the controller.
   *
   * @param value the new value
   */
  public void setLeftBumperButton(boolean value) {
    setButton(Gamepad.Button.LEFT_BUMPER, value);
  }

  /**
   * Change the value of the right bumper button on the controller.
   *
   * @param value the new value
   */
  public void setRightBumperButton(boolean value) {
    setButton(Gamepad.Button.RIGHT_BUMPER, value);
  }

  /**
   * Change the value of the D-pad up button on the controller.
   *
   * @param value the new value
   */
  public void setDpadUpButton(boolean value) {
    setButton(Gamepad.Button.DPAD_UP, value);
  }

  /**
   * Change the value of the D-pad down button on the controller.
   *
   * @param value the new value
   */
  public void setDpadDownButton(boolean value) {
    setButton(Gamepad.Button.DPAD_DOWN, value);
  }

  /**
   * Change the value of the D-pad left button on the controller.
   *
   * @param value the new value
   */
  public void setDpadLeftButton(boolean value) {
    setButton(Gamepad.Button.DPAD_LEFT, value);
  }

  /**
   * Change the value of the D-pad right button on the controller.
   *
   * @param value the new value
   */
  public void setDpadRightButton(boolean value) {
    setButton(Gamepad.Button.DPAD_RIGHT, value);
  }

  /**
   * Change the value of the Miscellaneous 1 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc1Button(boolean value) {
    setButton(Gamepad.Button.MISC_1, value);
  }

  /**
   * Change the value of the Right Paddle 1 button on the controller.
   *
   * @param value the new value
   */
  public void setRightPaddle1Button(boolean value) {
    setButton(Gamepad.Button.RIGHT_PADDLE_1, value);
  }

  /**
   * Change the value of the Left Paddle 1 button on the controller.
   *
   * @param value the new value
   */
  public void setLeftPaddle1Button(boolean value) {
    setButton(Gamepad.Button.LEFT_PADDLE_1, value);
  }

  /**
   * Change the value of the Right Paddle 2 button on the controller.
   *
   * @param value the new value
   */
  public void setRightPaddle2Button(boolean value) {
    setButton(Gamepad.Button.RIGHT_PADDLE_2, value);
  }

  /**
   * Change the value of the Left Paddle 2 button on the controller.
   *
   * @param value the new value
   */
  public void setLeftPaddle2Button(boolean value) {
    setButton(Gamepad.Button.LEFT_PADDLE_2, value);
  }

  /**
   * Change the value of the Touchpad button on the controller.
   *
   * @param value the new value
   */
  public void setTouchpadButton(boolean value) {
    setButton(Gamepad.Button.TOUCHPAD, value);
  }

  /**
   * Change the value of the Miscellaneous 2 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc2Button(boolean value) {
    setButton(Gamepad.Button.MISC_2, value);
  }

  /**
   * Change the value of the Miscellaneous 3 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc3Button(boolean value) {
    setButton(Gamepad.Button.MISC_3, value);
  }

  /**
   * Change the value of the Miscellaneous 4 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc4Button(boolean value) {
    setButton(Gamepad.Button.MISC_4, value);
  }

  /**
   * Change the value of the Miscellaneous 5 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc5Button(boolean value) {
    setButton(Gamepad.Button.MISC_5, value);
  }

  /**
   * Change the value of the Miscellaneous 6 button on the controller.
   *
   * @param value the new value
   */
  public void setMisc6Button(boolean value) {
    setButton(Gamepad.Button.MISC_6, value);
  }
}
