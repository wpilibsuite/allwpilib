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

/*
 * Intentionally package private.
 */
/**
 * Provide access to the network communication data to / from the Driver Station.
 */
class SimDriverStation implements IDriverStation.WithPrivateMethods {
  /**
   * Slot for the analog module to read the battery
   */
  private static final int kBatterySlot = 1;
  /**
   * Analog channel to read the battery
   */
  private static final int kBatteryChannel = 7;
  /**
   * Number of Joystick Ports
   */
  private static final int kJoystickPorts = 6;
  /**
   * Number of Joystick Axes
   */
  private static final int kJoystickAxes = 6;
  /**
   * Convert from raw values to volts
   */
  private static final double kDSAnalogInScaling = 5.0 / 1023.0;

  private final Object m_dataSem;
  private boolean m_userInDisabled = false;
  private boolean m_userInAutonomous = false;
  private boolean m_userInTeleop = false;
  private boolean m_userInTest = false;
  private boolean m_newControlData;
  private GzDriverStation.DriverStation state;
  private Joystick joysticks[] = new Joystick[6];

  /**
   * DriverStation constructor.
   *
   * The single DriverStation instance is created statically with the instance static member
   * variable.
   */
  protected SimDriverStation() {
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


  @Override
  public void waitForData(long timeout) {
    synchronized (m_dataSem) {
      try {
        m_dataSem.wait(timeout);
      } catch (InterruptedException ex) {
      }
    }
  }

  @Override
  public double getBatteryVoltage() {
    return 12.0; // 12 volts all the time!
  }

  @Override
  public double getStickAxis(int stick, int axis) {
    if (stick < 0 || stick >= joysticks.length || joysticks[stick] == null) {
      return 0.0;
    }
    if (axis < 0 || axis >= kJoystickAxes || axis >= joysticks[stick].getAxesCount()) {
      return 0.0;
    }
    return joysticks[stick].getAxes(axis);
  }

  @Override
  public boolean getStickButton(int stick, byte button) {
    if (stick < 0 || stick >= joysticks.length || joysticks[stick] == null) {
      return false;
    }
    if (button < 1 || button > joysticks[stick].getButtonsCount()) {
      return false;
    }
    return joysticks[stick].getButtons(button - 1);
  }

  @Override
  public boolean getJoystickIsXbox(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public String getJoystickName(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getJoystickType(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getStickButtonCount(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getStickAxisCount(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getStickPOV(int stick, int pov) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getStickPOVCount(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public int getStickButtons(int stick) {
    throw new UnsupportedOperationException("Method unimplemented");
  }

  @Override
  public boolean isEnabled() {
    return state != null ? state.getEnabled() : false;
  }

  @Override
  public boolean isDisabled() {
    return !isEnabled();
  }

  @Override
  public boolean isAutonomous() {
    return state != null ? state.getState() == State.AUTO : false;
  }

  @Override
  public boolean isTest() {
    return state != null ? state.getState() == State.TEST : false;
  }

  @Override
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


  @Override
  public DriverStation.Alliance getAlliance() {
    return DriverStation.Alliance.Invalid;
  }

  @Override
  public boolean isSysActive() {
    return true;
  }

  @Override
  public boolean isBrownedOut() {
    return false;
  }

  @Override
  public int getLocation() {
    return -1; // TODO: always -1
  }


  @Override
  public boolean isFMSAttached() {
    return false;
  }

  @Override
  public boolean isDSAttached() {
    return true;
  }

  @Override
  public double getMatchTime() {
    return 0;
  }

  @Override
  public void InDisabled(boolean entering) {
    m_userInDisabled = entering;
  }

  @Override
  public void InAutonomous(boolean entering) {
    m_userInAutonomous = entering;
  }

  @Override
  public void InOperatorControl(boolean entering) {
    m_userInTeleop = entering;
  }

  @Override
  public void InTest(boolean entering) {
    m_userInTeleop = entering;
  }

  @Override
  public void reportErrorImpl(boolean isError, int code, String error, boolean printTrace, String
      locString, String traceString) {
    // no-op
  }
}
