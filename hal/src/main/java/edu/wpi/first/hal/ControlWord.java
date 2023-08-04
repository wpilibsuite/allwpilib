// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** A wrapper for the HALControlWord bitfield. */
public class ControlWord {
  private boolean m_enabled;
  private boolean m_autonomous;
  private boolean m_test;
  private boolean m_emergencyStop;
  private boolean m_fmsAttached;
  private boolean m_dsAttached;

  void update(
      boolean enabled,
      boolean autonomous,
      boolean test,
      boolean emergencyStop,
      boolean fmsAttached,
      boolean dsAttached) {
    m_enabled = enabled;
    m_autonomous = autonomous;
    m_test = test;
    m_emergencyStop = emergencyStop;
    m_fmsAttached = fmsAttached;
    m_dsAttached = dsAttached;
  }

  /**
   * Updates from an existing word.
   *
   * @param word word to update from
   */
  public void update(ControlWord word) {
    m_enabled = word.m_enabled;
    m_autonomous = word.m_autonomous;
    m_test = word.m_test;
    m_emergencyStop = word.m_emergencyStop;
    m_fmsAttached = word.m_fmsAttached;
    m_dsAttached = word.m_dsAttached;
  }

  public boolean getEnabled() {
    return m_enabled;
  }

  public boolean getAutonomous() {
    return m_autonomous;
  }

  public boolean getTest() {
    return m_test;
  }

  public boolean getEStop() {
    return m_emergencyStop;
  }

  public boolean getFMSAttached() {
    return m_fmsAttached;
  }

  public boolean getDSAttached() {
    return m_dsAttached;
  }
}
