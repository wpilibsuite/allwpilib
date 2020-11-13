/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.HashMap;
import java.util.Map;

import edu.wpi.first.hal.HAL;

/**
 * GenericHID Interface.
 */
public abstract class GenericHID {
  /**
   * Represents a rumble output on the JoyStick.
   */
  public enum RumbleType {
    kLeftRumble, kRightRumble
  }

  public enum HIDType {
    kUnknown(-1),
    kXInputUnknown(0),
    kXInputGamepad(1),
    kXInputWheel(2),
    kXInputArcadeStick(3),
    kXInputFlightStick(4),
    kXInputDancePad(5),
    kXInputGuitar(6),
    kXInputGuitar2(7),
    kXInputDrumKit(8),
    kXInputGuitar3(11),
    kXInputArcadePad(19),
    kHIDJoystick(20),
    kHIDGamepad(21),
    kHIDDriving(22),
    kHIDFlight(23),
    kHID1stPerson(24);

    @SuppressWarnings("MemberName")
    public final int value;
    @SuppressWarnings("PMD.UseConcurrentHashMap")
    private static final Map<Integer, HIDType> map = new HashMap<>();

    HIDType(int value) {
      this.value = value;
    }

    static {
      for (HIDType hidType : HIDType.values()) {
        map.put(hidType.value, hidType);
      }
    }

    public static HIDType of(int value) {
      return map.get(value);
    }
  }

  /**
   * Which hand the Human Interface Device is associated with.
   */
  public enum Hand {
    kLeft(0), kRight(1);

    @SuppressWarnings("MemberName")
    public final int value;

    Hand(int value) {
      this.value = value;
    }
  }

  private DriverStation m_ds;
  private final int m_port;
  private int m_outputs;
  private short m_leftRumble;
  private short m_rightRumble;

  public GenericHID(int port) {
    m_ds = DriverStation.getInstance();
    m_port = port;
  }

  /**
   * Get the x position of the HID.
   *
   * @return the x position of the HID
   */
  public final double getX() {
    return getX(Hand.kRight);
  }

  /**
   * Get the x position of HID.
   *
   * @param hand which hand, left or right
   * @return the x position
   */
  public abstract double getX(Hand hand);

  /**
   * Get the y position of the HID.
   *
   * @return the y position
   */
  public final double getY() {
    return getY(Hand.kRight);
  }

  /**
   * Get the y position of the HID.
   *
   * @param hand which hand, left or right
   * @return the y position
   */
  public abstract double getY(Hand hand);

  /**
   * Get the button value (starting at button 1).
   *
   * <p>The buttons are returned in a single 16 bit value with one bit representing the state of
   * each button. The appropriate button is returned as a boolean value.
   *
   * <p>This method returns true if the button is being held down at the time
   * that this method is being called.
   *
   * @param button The button number to be read (starting at 1)
   * @return The state of the button.
   */
  public boolean getRawButton(int button) {
    return m_ds.getStickButton(m_port, (byte) button);
  }

  /**
   * Whether the button was pressed since the last check. Button indexes begin at
   * 1.
   *
   * <p>This method returns true if the button went from not pressed to held down
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you press the button.
   *
   * @param button The button index, beginning at 1.
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRawButtonPressed(int button) {
    return m_ds.getStickButtonPressed(m_port, (byte) button);
  }

  /**
   * Whether the button was released since the last check. Button indexes begin at
   * 1.
   *
   * <p>This method returns true if the button went from held down to not pressed
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you release the button.
   *
   * @param button The button index, beginning at 1.
   * @return Whether the button was released since the last check.
   */
  public boolean getRawButtonReleased(int button) {
    return m_ds.getStickButtonReleased(m_port, button);
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(int axis) {
    return m_ds.getStickAxis(m_port, axis);
  }

  /**
   * Get the angle in degrees of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (eg right is 90,
   * upper-left is 315).
   *
   * @param pov The index of the POV to read (starting at 0)
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public int getPOV(int pov) {
    return m_ds.getStickPOV(m_port, pov);
  }

  public int getPOV() {
    return getPOV(0);
  }

  /**
   * Get the number of axes for the HID.
   *
   * @return the number of axis for the current HID
   */
  public int getAxisCount() {
    return m_ds.getStickAxisCount(m_port);
  }

  /**
   * For the current HID, return the number of POVs.
   */
  public int getPOVCount() {
    return m_ds.getStickPOVCount(m_port);
  }

  /**
   * For the current HID, return the number of buttons.
   */
  public int getButtonCount() {
    return m_ds.getStickButtonCount(m_port);
  }

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  public boolean isConnected() {
    return m_ds.isJoystickConnected(m_port);
  }

  /**
   * Get the type of the HID.
   *
   * @return the type of the HID.
   */
  public HIDType getType() {
    return HIDType.of(m_ds.getJoystickType(m_port));
  }

  /**
   * Get the name of the HID.
   *
   * @return the name of the HID.
   */
  public String getName() {
    return m_ds.getJoystickName(m_port);
  }

  /**
   * Get the axis type of a joystick axis.
   *
   * @return the axis type of a joystick axis.
   */
  public int getAxisType(int axis) {
    return m_ds.getJoystickAxisType(m_port, axis);
  }

  /**
   * Get the port number of the HID.
   *
   * @return The port number of the HID.
   */
  public int getPort() {
    return m_port;
  }

  /**
   * Set a single HID output value for the HID.
   *
   * @param outputNumber The index of the output to set (1-32)
   * @param value        The value to set the output to
   */
  public void setOutput(int outputNumber, boolean value) {
    m_outputs = (m_outputs & ~(1 << (outputNumber - 1))) | ((value ? 1 : 0) << (outputNumber - 1));
    HAL.setJoystickOutputs((byte) m_port, m_outputs, m_leftRumble, m_rightRumble);
  }

  /**
   * Set all HID output values for the HID.
   *
   * @param value The 32 bit output value (1 bit for each output)
   */
  public void setOutputs(int value) {
    m_outputs = value;
    HAL.setJoystickOutputs((byte) m_port, m_outputs, m_leftRumble, m_rightRumble);
  }

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type  Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
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
    HAL.setJoystickOutputs((byte) m_port, m_outputs, m_leftRumble, m_rightRumble);
  }
}
