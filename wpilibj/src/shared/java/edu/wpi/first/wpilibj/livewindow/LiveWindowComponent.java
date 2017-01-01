/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.livewindow;

/**
 * A LiveWindow component is a device (sensor or actuator) that should be added to the
 * SmartDashboard in test mode. The components are cached until the first time the robot enters Test
 * mode. This allows the components to be inserted, then renamed.
 */
/*
 * This class is intentionally package private.
 */
class LiveWindowComponent {

  String m_subsystem;
  String m_name;
  boolean m_isSensor;

  public LiveWindowComponent(String subsystem, String name, boolean isSensor) {
    m_subsystem = subsystem;
    m_name = name;
    m_isSensor = isSensor;
  }

  public String getName() {
    return m_name;
  }

  public String getSubsystem() {
    return m_subsystem;
  }

  public boolean isSensor() {
    return m_isSensor;
  }
}
