// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.GenericHID;
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
  }

  /**
   * Change the X value of the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  public void setX(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftX.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightX.value, value);
    }
  }

  /**
   * Change the Y value of the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  public void setY(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftY.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightY.value, value);
    }
  }

  /**
   * Change the value of a trigger axis on the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  public void setTriggerAxis(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftTrigger.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightTrigger.value, value);
    }
  }

  /**
   * Change the value of a bumper on the joystick.
   *
   * @param hand the joystick hand
   * @param state the new value
   */
  public void setBumper(GenericHID.Hand hand, boolean state) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawButton(XboxController.Button.kBumperLeft.value, state);
    } else {
      setRawButton(XboxController.Button.kBumperRight.value, state);
    }
  }

  /**
   * Change the value of a button on the joystick.
   *
   * @param hand the joystick hand
   * @param state the new value
   */
  public void setStickButton(GenericHID.Hand hand, boolean state) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawButton(XboxController.Button.kStickLeft.value, state);
    } else {
      setRawButton(XboxController.Button.kStickRight.value, state);
    }
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
