package edu.wpi.first.wpilibj;

/*
 * Intentionally package private.
 */
/**
 * Enforces the DriverStation api.
 */
interface IDriverStation extends RobotState.Interface {

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage.
   */
  double getBatteryVoltage();

  /**
   * Wait for new data or for timeout, which ever comes first.  If timeout is 0, wait for new data
   * only.
   *
   * @param timeout The maximum time in milliseconds to wait.
   */
  void waitForData(long timeout);

  /**
   * Wait for new data from the driver station.
   */
  default void waitForData() {
    waitForData(0);
  }

  /**
   * Get the value of the axis on a joystick. This depends on the mapping of the joystick
   * connected to the specified port.
   *
   * @param stick The joystick to read.
   * @param axis  The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick.
   */
  double getStickAxis(int stick, int axis);

  /**
   * The state of one joystick button. Button indexes begin at 1.
   *
   * @param stick  The joystick to read.
   * @param button The button index, beginning at 1.
   * @return The state of the joystick button.
   */
  boolean getStickButton(int stick, byte button);


  /**
   * Gets the value of isXbox on a joystick.
   *
   * @param stick The joystick port number
   * @return A boolean that returns the value of isXbox
   */
  boolean getJoystickIsXbox(int stick);

  /**
   * Gets the name of the joystick at a port.
   *
   * @param stick The joystick port number
   * @return The value of name
   */
  String getJoystickName(int stick);

  /**
   * Gets the value of type on a joystick.
   *
   * @param stick The joystick port number
   * @return The value of type
   */
  int getJoystickType(int stick);

  /**
   * Gets the number of buttons on a joystick.
   *
   * @param stick The joystick port number
   * @return The number of buttons on the indicated joystick
   */
  int getStickButtonCount(int stick);

  /**
   * Returns the number of axes on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of axes on the indicated joystick
   */
  int getStickAxisCount(int stick);

  /**
   * Get the state of a POV on the joystick.
   *
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  int getStickPOV(int stick, int pov);

  /**
   * Returns the number of POVs on a given joystick port.
   *
   * @param stick The joystick port number
   * @return The number of POVs on the indicated joystick
   */
  int getStickPOVCount(int stick);

  /**
   * The state of the buttons on the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  int getStickButtons(final int stick);


  /**
   * Get the current alliance from the FMS.
   *
   * @return the current alliance
   */
  DriverStation.Alliance getAlliance();

  /**
   * Gets a value indicating whether the FPGA outputs are enabled. The outputs may be disabled if
   * the robot is disabled or e-stopped, the watchdog has expired, or if the roboRIO browns out.
   *
   * @return True if the FPGA outputs are enabled.
   */
  boolean isSysActive();

  /**
   * Check if the system is browned out.
   *
   * @return True if the system is browned out
   */
  boolean isBrownedOut();

  /**
   * Has a new control packet from the driver station arrived since the last time this function
   * was called?
   *
   * @return True if the control data has been updated since the last call.
   */
  boolean isNewControlData();

  /**
   * Gets the location of the team's driver station controls.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  int getLocation();

  /**
   * Is the driver station attached to a Field Management System(FMS)? Note: This does not work
   * with the Blue DS.
   *
   * @return True if the robot is competing on a field being controlled by a FMS
   */
  boolean isFMSAttached();

  /**
   * Is the driver station attached.
   */
  boolean isDSAttached();

  /**
   * Return the approximate match time The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used
   * to dispute ref calls or guarantee that a function will trigger before the match ends) The
   * Practice Match function of the DS approximates the behaviour seen on the field.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  double getMatchTime();

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting disabled code; if false, leaving disabled code
   */
  @SuppressWarnings("MethodName")
  void InDisabled(boolean entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting autonomous code; if false, leaving autonomous code
   */
  @SuppressWarnings("MethodName")
  void InAutonomous(boolean entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   */
  @SuppressWarnings("MethodName")
  void InOperatorControl(boolean entering);

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only.
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  @SuppressWarnings("MethodName")
  void InTest(boolean entering);

  /**
   * Methods that don't need to be exposed as part of the public API.
   */
  interface WithPrivateMethods extends IDriverStation {

    void reportErrorImpl(boolean isError, int code, String error,
                         boolean printTrace, String locString, String traceString);
  }
}
