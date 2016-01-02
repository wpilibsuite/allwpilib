/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.communication;

/**
 * A wrapper for the HALControlWord bitfield
 */
public class HALControlWord {
  private boolean m_enabled;
  private boolean m_autonomous;
  private boolean m_test;
  private boolean m_eStop;
  private boolean m_fmsAttached;
  private boolean m_dsAttached;

  protected HALControlWord(boolean enabled, boolean autonomous, boolean test, boolean eStop,
      boolean fmsAttached, boolean dsAttached) {
    m_enabled = enabled;
    m_autonomous = autonomous;
    m_test = test;
    m_eStop = eStop;
    m_fmsAttached = fmsAttached;
    m_dsAttached = dsAttached;
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
    return m_eStop;
  }

  public boolean getFMSAttached() {
    return m_fmsAttached;
  }

  public boolean getDSAttached() {
    return m_dsAttached;
  }


}
