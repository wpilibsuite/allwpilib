/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

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

    private HIDType(int value) {
      this.value = value;
    }
  }

  /**
   * Which hand the Human Interface Device is associated with.
   */
  public enum Hand {
    kLeft(0), kRight(1);

    @SuppressWarnings("MemberName")
    public final int value;

    private Hand(int value) {
      this.value = value;
    }
  }

  private final int m_port;

  public GenericHID(int port) {
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
   * Get the raw axis.
   *
   * @param which index of the axis
   * @return the raw value of the selected axis
   */
  public abstract double getRawAxis(int which);

  /**
   * Is the given button pressed.
   *
   * @param button which button number
   * @return true if the button is pressed
   */
  public abstract boolean getRawButton(int button);

  /**
   * Get the angle in degrees of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (eg right is 90,
   * upper-left is 315).
   *
   * @param pov The index of the POV to read (starting at 0)
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public abstract int getPOV(int pov);

  public int getPOV() {
    return getPOV(0);
  }

  /**
   * For the current HID, return the number of POVs.
   */
  public abstract int getPOVCount();

  /**
   * Get the port number of the HID.
   *
   * @return The port number of the HID.
   */
  public int getPort() {
    return m_port;
  }

  /**
   * Get the type of the HID.
   *
   * @return the type of the HID.
   */
  public abstract HIDType getType();

  /**
   * Get the name of the HID.
   *
   * @return the name of the HID.
   */
  public abstract String getName();

  /**
   * Set a single HID output value for the HID.
   *
   * @param outputNumber The index of the output to set (1-32)
   * @param value        The value to set the output to
   */
  public abstract void setOutput(int outputNumber, boolean value);

  /**
   * Set all HID output values for the HID.
   *
   * @param value The 32 bit output value (1 bit for each output)
   */
  public abstract void setOutputs(int value);

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type  Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  public abstract void setRumble(RumbleType type, double value);
}
