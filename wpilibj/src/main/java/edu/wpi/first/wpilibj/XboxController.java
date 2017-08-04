/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * Handle input from Xbox 360 or Xbox One controllers connected to the Driver Station.
 *
 * <p>This class handles Xbox input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returend. There is a single class instance for each controller
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class XboxController extends GamepadBase {
  private DriverStation m_ds;
  private int m_outputs;
  private short m_leftRumble;
  private short m_rightRumble;

  /**
   * Construct an instance of a joystick. The joystick index is the USB port on the drivers
   * station.
   *
   * @param port The port on the Driver Station that the joystick is plugged into.
   */
  public XboxController(final int port) {
    super(port);
    m_ds = DriverStation.getInstance();

    // HAL.report(tResourceType.kResourceType_XboxController, port);
    HAL.report(tResourceType.kResourceType_Joystick, port);
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
      return getRawAxis(0);
    } else {
      return getRawAxis(4);
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
      return getRawAxis(1);
    } else {
      return getRawAxis(5);
    }
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(final int axis) {
    return m_ds.getStickAxis(getPort(), axis);
  }

  /**
   * Read the value of the bumper button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  @Override
  public boolean getBumper(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawButton(5);
    } else {
      return getRawButton(6);
    }
  }

  /**
   * This is not supported for the XboxController. This method is only here to complete the
   * GenericHID interface.
   *
   * @param hand This parameter is ignored for the Joystick class and is only here to complete the
   *             GenericHID interface.
   * @return The state of the trigger (always false)
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public boolean getTrigger(Hand hand) {
    return false;
  }

  /**
   * This is not supported for the XboxController. This method is only here to complete the
   * GenericHID interface.
   *
   * @param hand This parameter is ignored for the Joystick class and is only here to complete the
   *             GenericHID interface.
   * @return The state of the top button (always false)
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public boolean getTop(Hand hand) {
    return false;
  }

  /**
   * Get the button value (starting at button 1).
   *
   * <p>The appropriate button is returned as a boolean value.
   *
   * @param button The button number to be read (starting at 1).
   * @return The state of the button.
   */
  public boolean getRawButton(final int button) {
    return m_ds.getStickButton(getPort(), (byte) button);
  }

  /**
   * Get the trigger axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The trigger axis value of the controller.
   */
  public double getTriggerAxis(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawAxis(2);
    } else {
      return getRawAxis(3);
    }
  }

  /**
   * Read the value of the A button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getAButton() {
    return getRawButton(1);
  }

  /**
   * Read the value of the B button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBButton() {
    return getRawButton(2);
  }

  /**
   * Read the value of the X button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getXButton() {
    return getRawButton(3);
  }

  /**
   * Read the value of the Y button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getYButton() {
    return getRawButton(4);
  }

  /**
   * Read the value of the stick button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  @Override
  public boolean getStickButton(Hand hand) {
    if (hand.equals(Hand.kLeft)) {
      return getRawButton(9);
    } else {
      return getRawButton(10);
    }
  }

  /**
   * Read the value of the back button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBackButton() {
    return getRawButton(7);
  }

  /**
   * Read the value of the start button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getStartButton() {
    return getRawButton(8);
  }

  @Override
  public int getPOV(int pov) {
    return m_ds.getStickPOV(getPort(), pov);
  }

  @Override
  public int getPOVCount() {
    return m_ds.getStickPOVCount(getPort());
  }

  @Override
  public HIDType getType() {
    return HIDType.values()[m_ds.getJoystickType(getPort())];
  }

  @Override
  public String getName() {
    return m_ds.getJoystickName(getPort());
  }

  @Override
  public void setOutput(int outputNumber, boolean value) {
    m_outputs = (m_outputs & ~(1 << (outputNumber - 1))) | ((value ? 1 : 0) << (outputNumber - 1));
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }

  @Override
  public void setOutputs(int value) {
    m_outputs = value;
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }

  @Override
  public void setRumble(RumbleType type, double value) {
    if (value < 0) {
      value = 0;
    } else if (value > 1) {
      value = 1;
    }
    if (type == RumbleType.kLeftRumble) {
      m_leftRumble = (short) (value * 65535);
    } else {
      m_rightRumble = (short) (value * 65535);
    }
    HAL.setJoystickOutputs((byte) getPort(), m_outputs, m_leftRumble, m_rightRumble);
  }
}
