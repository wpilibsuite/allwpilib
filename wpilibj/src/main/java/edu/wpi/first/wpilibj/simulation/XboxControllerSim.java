/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.XboxController;

/**
 * Class to control a simulated Xbox 360 or Xbox One controller.
 */
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

  public void setX(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftX.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightX.value, value);
    }
  }

  public void setY(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftY.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightY.value, value);
    }
  }

  public void setTriggerAxis(GenericHID.Hand hand, double value) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawAxis(XboxController.Axis.kLeftTrigger.value, value);
    } else {
      setRawAxis(XboxController.Axis.kRightTrigger.value, value);
    }
  }

  public void setBumper(GenericHID.Hand hand, boolean state) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawButton(XboxController.Button.kBumperLeft.value, state);
    } else {
      setRawButton(XboxController.Button.kBumperRight.value, state);
    }
  }

  public void setStickButton(GenericHID.Hand hand, boolean state) {
    if (hand.equals(GenericHID.Hand.kLeft)) {
      setRawButton(XboxController.Button.kStickLeft.value, state);
    } else {
      setRawButton(XboxController.Button.kStickRight.value, state);
    }
  }

  public void setAButton(boolean state) {
    setRawButton(XboxController.Button.kA.value, state);
  }

  public void setBButton(boolean state) {
    setRawButton(XboxController.Button.kB.value, state);
  }

  public void setXButton(boolean state) {
    setRawButton(XboxController.Button.kX.value, state);
  }

  public void setYButton(boolean state) {
    setRawButton(XboxController.Button.kY.value, state);
  }

  public void setBackButton(boolean state) {
    setRawButton(XboxController.Button.kBack.value, state);
  }

  public void setStartButton(boolean state) {
    setRawButton(XboxController.Button.kStart.value, state);
  }
}
