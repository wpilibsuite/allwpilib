package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
public final class DriverStation implements IDriverStation {
  /**
   * Number of Joystick Ports.
   */
  public static final int kJoystickPorts = 6;

  /**
   * The robot alliance that the robot is a part of.
   */
  public enum Alliance {
    Red, Blue, Invalid
  }

  private static DriverStation instance;

  /**
   * Sets the real implementation of the driver station to be used.
   */
  @SuppressWarnings("MethodName")
  public static void SetImplementation(IDriverStation.WithPrivateMethods driverStation) {
    instance = new DriverStation(driverStation);
  }

  /**
   * Gets an instance of the DriverStation.
   *
   * @return The DriverStation.
   */
  public static DriverStation getInstance() {
    if (instance != null) {
      return instance;
    } else {
      throw new BaseSystemNotInitializedException(IDriverStation.WithPrivateMethods.class,
          DriverStation.class);
    }
  }

  private final IDriverStation.WithPrivateMethods m_ds;

  private DriverStation(IDriverStation.WithPrivateMethods driverStation) {
    m_ds = driverStation;
  }

  @Override
  public double getBatteryVoltage() {
    return m_ds.getBatteryVoltage();
  }

  @Override
  public void waitForData(long timeout) {
    m_ds.waitForData();
  }

  @Override
  public double getStickAxis(int stick, int axis) {
    return m_ds.getStickAxis(stick, axis);
  }

  @Override
  public boolean getStickButton(int stick, byte button) {
    return m_ds.getStickButton(stick, button);
  }

  @Override
  public boolean getJoystickIsXbox(int stick) {
    return m_ds.getJoystickIsXbox(stick);
  }

  @Override
  public String getJoystickName(int stick) {
    return m_ds.getJoystickName(stick);
  }

  @Override
  public int getJoystickType(int stick) {
    return m_ds.getJoystickType(stick);
  }

  @Override
  public int getStickButtonCount(int stick) {
    return m_ds.getStickButtonCount(stick);
  }

  @Override
  public int getStickAxisCount(int stick) {
    return m_ds.getStickAxisCount(stick);
  }

  @Override
  public int getStickPOV(int stick, int pov) {
    return m_ds.getStickPOV(stick, pov);
  }

  @Override
  public int getStickPOVCount(int stick) {
    return m_ds.getStickPOVCount(stick);
  }

  @Override
  public int getStickButtons(int stick) {
    return m_ds.getStickButtons(stick);
  }

  @Override
  public Alliance getAlliance() {
    return m_ds.getAlliance();
  }

  @Override
  public boolean isSysActive() {
    return m_ds.isSysActive();
  }

  @Override
  public boolean isBrownedOut() {
    return m_ds.isBrownedOut();
  }

  @Override
  public boolean isNewControlData() {
    return m_ds.isNewControlData();
  }

  @Override
  public int getLocation() {
    return m_ds.getLocation();
  }

  @Override
  public boolean isFMSAttached() {
    return m_ds.isFMSAttached();
  }

  @Override
  public boolean isDSAttached() {
    return m_ds.isDSAttached();
  }

  @Override
  public double getMatchTime() {
    return m_ds.getMatchTime();
  }

  @Override
  public void InDisabled(boolean entering) {
    m_ds.InDisabled(entering);
  }

  @Override
  public void InAutonomous(boolean entering) {
    m_ds.InAutonomous(entering);
  }

  @Override
  public void InOperatorControl(boolean entering) {
    m_ds.InOperatorControl(entering);
  }

  @Override
  public void InTest(boolean entering) {
    m_ds.InTest(entering);
  }

  @Override
  public boolean isDisabled() {
    return m_ds.isDisabled();
  }

  @Override
  public boolean isEnabled() {
    return m_ds.isEnabled();
  }

  @Override
  public boolean isOperatorControl() {
    return m_ds.isOperatorControl();
  }

  @Override
  public boolean isAutonomous() {
    return m_ds.isAutonomous();
  }

  @Override
  public boolean isTest() {
    return m_ds.isTest();
  }

  /**
   * Report error to Driver Station. Also prints error to System.err Optionally appends Stack trace
   * to error message.
   *
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    reportErrorImpl(true, 1, error, printTrace);
  }

  /**
   * Report warning to Driver Station. Also prints error to System.err Optionally appends Stack
   * trace to warning message.
   *
   * @param printTrace If true, append stack trace to warning string
   */
  public static void reportWarning(String error, boolean printTrace) {
    reportErrorImpl(false, 1, error, printTrace);
  }

  private static void reportErrorImpl(boolean isError, int code, String error,
                                      boolean printTrace) {
    StackTraceElement[] traces = Thread.currentThread().getStackTrace();
    String locString;
    if (traces.length > 3) {
      locString = traces[3].toString();
    } else {
      locString = "";
    }
    boolean haveLoc = false;
    String traceString = " at ";
    for (int i = 3; i < traces.length; i++) {
      String loc = traces[i].toString();
      traceString += loc + "\n";
      // get first user function
      if (!haveLoc && !loc.startsWith("edu.wpi.first.wpilibj")) {
        locString = loc;
        haveLoc = true;
      }
    }
    getInstance().m_ds.reportErrorImpl(isError, code, error, printTrace, locString, traceString);
  }
}
