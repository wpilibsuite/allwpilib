/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;

/**
 * Handle input from Xbox 360 or Xbox One controllers connected to the Driver Station.
 *
 * <p>This class handles Xbox input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each controller
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class XboxController extends GenericHID {
  /**
   * Represents a digital button on an XboxController.
   */
  public enum Button {
    kBumperLeft(5),
    kBumperRight(6),
    kStickLeft(9),
    kStickRight(10),
    kA(1),
    kB(2),
    kX(3),
    kY(4),
    kBack(7),
    kStart(8);

    @SuppressWarnings({"MemberName", "PMD.SingularField"})
    public final int value;

    Button(int value) {
      this.value = value;
    }
  }

  /**
   * Represents an axis on an XboxController.
   */
  public enum Axis {
    kLeftX(0),
    kRightX(4),
    kLeftY(1),
    kRightY(5),
    kLeftTrigger(2),
    kRightTrigger(3);

    @SuppressWarnings({"MemberName", "PMD.SingularField"})
    public final int value;

    Axis(int value) {
      this.value = value;
    }
  }

  /**
   * Construct an instance of a joystick. The joystick index is the USB port on the drivers
   * station.
   *
   * @param port The port on the Driver Station that the joystick is plugged into.
   */
  public XboxController(final int port) {
    super(port);

    HAL.report(tResourceType.kResourceType_XboxController, port + 1);
  }

  /**
   * Get the X axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The X axis value of the controller.
   */
  @Override
  public double getX(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawAxis(Axis.kLeftX.value);
    } else {
      return getRawAxis(Axis.kRightX.value);
    }
  }

  /**
   * Get the Y axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The Y axis value of the controller.
   */
  @Override
  public double getY(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawAxis(Axis.kLeftY.value);
    } else {
      return getRawAxis(Axis.kRightY.value);
    }
  }

  /**
   * Get the trigger axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The trigger axis value of the controller.
   */
  public double getTriggerAxis(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawAxis(Axis.kLeftTrigger.value);
    } else {
      return getRawAxis(Axis.kRightTrigger.value);
    }
  }

  /**
   * Read the value of the bumper button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  public boolean getBumper(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawButton(Button.kBumperLeft.value);
    } else {
      return getRawButton(Button.kBumperRight.value);
    }
  }

  /**
   * Whether the bumper was pressed since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBumperPressed(Hand hand) {
    if (hand == Hand.kLeft) {
      return getRawButtonPressed(Button.kBumperLeft.value);
    } else {
      return getRawButtonPressed(Button.kBumperRight.value);
    }
  }

  /**
   * Whether the bumper was released since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was released since the last check.
   */
  public boolean getBumperReleased(Hand hand) {
    if (hand == Hand.kLeft) {
      return getRawButtonReleased(Button.kBumperLeft.value);
    } else {
      return getRawButtonReleased(Button.kBumperRight.value);
    }
  }

  /**
   * Read the value of the stick button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  public boolean getStickButton(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawButton(Button.kStickLeft.value);
    } else {
      return getRawButton(Button.kStickRight.value);
    }
  }

  /**
   * Whether the stick button was pressed since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was pressed since the last check.
   */
  public boolean getStickButtonPressed(Hand hand) {
    if (hand == Hand.kLeft) {
      return getRawButtonPressed(Button.kStickLeft.value);
    } else {
      return getRawButtonPressed(Button.kStickRight.value);
    }
  }

  /**
   * Whether the stick button was released since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was released since the last check.
   */
  public boolean getStickButtonReleased(Hand hand) {
    if (hand == Hand.kLeft) {
      return getRawButtonReleased(Button.kStickLeft.value);
    } else {
      return getRawButtonReleased(Button.kStickRight.value);
    }
  }

  /**
   * Read the value of the A button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getAButton() {
    return getRawButton(Button.kA.value);
  }

  /**
   * Whether the A button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getAButtonPressed() {
    return getRawButtonPressed(Button.kA.value);
  }

  /**
   * Whether the A button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getAButtonReleased() {
    return getRawButtonReleased(Button.kA.value);
  }

  /**
   * Read the value of the B button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBButton() {
    return getRawButton(Button.kB.value);
  }

  /**
   * Whether the B button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBButtonPressed() {
    return getRawButtonPressed(Button.kB.value);
  }

  /**
   * Whether the B button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBButtonReleased() {
    return getRawButtonReleased(Button.kB.value);
  }

  /**
   * Read the value of the X button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getXButton() {
    return getRawButton(Button.kX.value);
  }

  /**
   * Whether the X button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getXButtonPressed() {
    return getRawButtonPressed(Button.kX.value);
  }

  /**
   * Whether the X button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getXButtonReleased() {
    return getRawButtonReleased(Button.kX.value);
  }

  /**
   * Read the value of the Y button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getYButton() {
    return getRawButton(Button.kY.value);
  }

  /**
   * Whether the Y button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getYButtonPressed() {
    return getRawButtonPressed(Button.kY.value);
  }

  /**
   * Whether the Y button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getYButtonReleased() {
    return getRawButtonReleased(Button.kY.value);
  }

  /**
   * Read the value of the back button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBackButton() {
    return getRawButton(Button.kBack.value);
  }

  /**
   * Whether the back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBackButtonPressed() {
    return getRawButtonPressed(Button.kBack.value);
  }

  /**
   * Whether the back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBackButtonReleased() {
    return getRawButtonReleased(Button.kBack.value);
  }

  /**
   * Read the value of the start button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getStartButton() {
    return getRawButton(Button.kStart.value);
  }

  /**
   * Whether the start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getStartButtonPressed() {
    return getRawButtonPressed(Button.kStart.value);
  }

  /**
   * Whether the start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getStartButtonReleased() {
    return getRawButtonReleased(Button.kStart.value);
  }
}
