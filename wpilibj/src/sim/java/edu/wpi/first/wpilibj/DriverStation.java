/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.gazebosim.transport.SubscriberCallback;

import edu.wpi.first.wpilibj.simulation.MainNode;
import gazebo.msgs.GzDriverStation;
import gazebo.msgs.GzDriverStation.DriverStation.State;
import gazebo.msgs.GzJoystick.Joystick;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
public class DriverStation implements RobotState.Interface {
  /**
   * Slot for the analog module to read the battery
   */
  public static final int kBatterySlot = 1;
  /**
   * Analog channel to read the battery
   */
  public static final int kBatteryChannel = 7;
  /**
   * Number of Joystick Ports
   */
  public static final int kJoystickPorts = 6;
  /**
   * Number of Joystick Axes
   */
  public static final int kJoystickAxes = 6;
  /**
   * Convert from raw values to volts
   */
  public static final double kDSAnalogInScaling = 5.0 / 1023.0;

  /**
   * The robot alliance that the robot is a part of
   */
  public static class Alliance {

    /**
     * The integer value representing this enumeration.
     */
    public final int value;
    /**
     * The Alliance name.
     */
    public final String name;
    public static final int kRed_val = 0;
    public static final int kBlue_val = 1;
    public static final int kInvalid_val = 2;
    /**
     * alliance: Red
     */
    public static final Alliance kRed = new Alliance(kRed_val, "Red");
    /**
     * alliance: Blue
     */
    public static final Alliance kBlue = new Alliance(kBlue_val, "Blue");
    /**
     * alliance: Invalid
     */
    public static final Alliance kInvalid = new Alliance(kInvalid_val, "invalid");

    private Alliance(int value, String name) {
      this.value = value;
      this.name = name;
    }
  } /* Alliance */


  private static DriverStation instance = new DriverStation();
  private final Object m_dataSem;
  private boolean m_userInDisabled = false;
  private boolean m_userInAutonomous = false;
  private boolean m_userInTeleop = false;
  private boolean m_userInTest = false;
  private boolean m_newControlData;
  private GzDriverStation.DriverStation state;
  private Joystick joysticks[] = new Joystick[6];

  /**
   * Gets an instance of the DriverStation
   *
   * @return The DriverStation.
   */
  public static DriverStation getInstance() {
    return DriverStation.instance;
  }

  /**
   * DriverStation constructor.
   *
   * The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  protected DriverStation() {
    m_dataSem = new Object();

    MainNode.subscribe("ds/state", GzDriverStation.DriverStation.getDefaultInstance(),
        new SubscriberCallback<GzDriverStation.DriverStation>() {
          @Override
          public void callback(GzDriverStation.DriverStation msg) {
            state = msg;
            m_newControlData = true;
            synchronized (m_dataSem) {
              m_dataSem.notifyAll();
            }
          }
        }
    );

    for (int i = 0; i < 6; i++) {
      final int j = i;
      MainNode.subscribe("ds/joysticks/" + i, Joystick.getDefaultInstance(),
          new SubscriberCallback<Joystick>() {
            @Override
            public void callback(Joystick msg) {
              synchronized (m_dataSem) {
                joysticks[j] = msg;
              }
            }
          }
      );
    }
  }

  /**
   * Wait for new data from the driver station.
   */
  public void waitForData() {
    waitForData(0);
  }

  /**
   * Wait for new data or for timeout, which ever comes first.  If timeout is 0, wait for new data
   * only.
   *
   * @param timeout The maximum time in milliseconds to wait.
   */
  public void waitForData(long timeout) {
    synchronized (m_dataSem) {
      try {
        m_dataSem.wait(timeout);
      } catch (InterruptedException ex) {
      }
    }
  }

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage.
   */
  public double getBatteryVoltage() {
    return 12.0; // 12 volts all the time!
  }

  /**
   * Get the value of the axis on a joystick. This depends on the mapping of the joystick connected
   * to the specified port.
   *
   * @param stick The joystick to read.
   * @param axis  The analog axis value to read from the joystick.
   * @return The value of the axis on the joystick.
   */
  public double getStickAxis(int stick, int axis) {
    if (stick < 0 || stick >= joysticks.length || joysticks[stick] == null) {
      return 0.0;
    }
    if (axis < 0 || axis >= kJoystickAxes || axis >= joysticks[stick].getAxesCount()) {
      return 0.0;
    }
    return joysticks[stick].getAxes(axis);
  }

  /**
   * The state of the buttons on the joystick. 12 buttons (4 msb are unused) from the joystick.
   *
   * @param stick The joystick to read.
   * @return The state of the buttons on the joystick.
   */
  public boolean getStickButton(int stick, int button) {
    if (stick < 0 || stick >= joysticks.length || joysticks[stick] == null) {
      return false;
    }
    if (button < 1 || button > joysticks[stick].getButtonsCount()) {
      return false;
    }
    return joysticks[stick].getButtons(button - 1);
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be enabled.
   *
   * @return True if the robot is enabled, false otherwise.
   */
  public boolean isEnabled() {
    return state != null ? state.getEnabled() : false;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be disabled.
   *
   * @return True if the robot should be disabled, false otherwise.
   */
  public boolean isDisabled() {
    return !isEnabled();
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * autonomous mode.
   *
   * @return True if autonomous mode should be enabled, false otherwise.
   */
  public boolean isAutonomous() {
    return state != null ? state.getState() == State.AUTO : false;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in test
   * mode.
   *
   * @return True if test mode should be enabled, false otherwise.
   */
  public boolean isTest() {
    return state != null ? state.getState() == State.TEST : false;
  }

  /**
   * Gets a value indicating whether the Driver Station requires the robot to be running in
   * operator-controlled mode.
   *
   * @return True if operator-controlled mode should be enabled, false otherwise.
   */
  public boolean isOperatorControl() {
    return !(isAutonomous() || isTest());
  }

  /**
   * Has a new control packet from the driver station arrived since the last time this function was
   * called?
   *
   * @return True if the control data has been updated since the last call.
   */
  public synchronized boolean isNewControlData() {
    boolean result = m_newControlData;
    m_newControlData = false;
    return result;
  }

  /**
   * Get the current alliance from the FMS
   *
   * @return the current alliance
   */
  public Alliance getAlliance() {
    switch ('I') { // TODO: Always invalid
      case 'R':
        return Alliance.kRed;
      case 'B':
        return Alliance.kBlue;
      default:
        return Alliance.kInvalid;
    }
  }

  /**
   * Gets the location of the team's driver station controls.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  public int getLocation() {
    return -1; // TODO: always -1
  }

  /**
   * Return the team number that the Driver Station is configured for
   *
   * @return The team number
   */
  public int getTeamNumber() {
    return 348; // TODO
  }

  /**
   * Is the driver station attached to a Field Management System? Note: This does not work with the
   * Blue DS.
   *
   * @return True if the robot is competing on a field being controlled by a Field Management System
   */
  public boolean isFMSAttached() {
    return false;
  }

  /**
   * Report error to Driver Station. Also prints error to System.err Optionally appends Stack trace
   * to error message
   *
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    String errorString = error;
    if (printTrace) {
      errorString += " at ";
      StackTraceElement[] traces = Thread.currentThread().getStackTrace();
      for (int i = 2; i < traces.length; i++) {
        errorString += traces[i].toString() + "\n";
      }
    }
    System.err.println(errorString);
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only
   *
   * @param entering If true, starting disabled code; if false, leaving disabled code
   */
  public void InDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only
   *
   * @param entering If true, starting autonomous code; if false, leaving autonomous code
   */
  public void InAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only
   *
   * @param entering If true, starting teleop code; if false, leaving teleop code
   */
  public void InOperatorControl(boolean entering) {
    m_userInTeleop = entering;
  }

  /**
   * Only to be used to tell the Driver Station what code you claim to be executing for diagnostic
   * purposes only
   *
   * @param entering If true, starting test code; if false, leaving test code
   */
  public void InTest(boolean entering) {
    m_userInTeleop = entering;
  }
}
