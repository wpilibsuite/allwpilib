/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.SolenoidJNI;

/**
 * SolenoidBase class is the common base class for the Solenoid and DoubleSolenoid classes.
 */
public abstract class SolenoidBase extends SensorBase {
  protected final byte m_moduleNumber; // /< The number of the solenoid module being
  // used.

  /**
   * Constructor.
   *
   * @param moduleNumber The PCM CAN ID
   */
  public SolenoidBase(final int moduleNumber) {
    m_moduleNumber = (byte)moduleNumber;
  }

  /**
   * Read all 8 solenoids from the module used by this solenoid as a single byte.
   *
   * @return The current value of all 8 solenoids on this module.
   */
  public byte getAll() {
    return SolenoidJNI.getAllSolenoids(m_moduleNumber);
  }

  /**
   * Reads complete solenoid blacklist for all 8 solenoids as a single byte. If a solenoid is
   * shorted, it is added to the blacklist and disabled until power cycle, or until faults are
   * cleared.
   *
   * @return The solenoid blacklist of all 8 solenoids on the module.
   * @see #clearAllPCMStickyFaults()
   */
  public byte getPCMSolenoidBlackList() {
    return (byte) SolenoidJNI.getPCMSolenoidBlackList(m_moduleNumber);
  }

  /**
   * If true, the common highside solenoid voltage rail is too low, most likely a solenoid channel
   * is shorted.
   *
   * @return true if PCM sticky fault is set
   */
  public boolean getPCMSolenoidVoltageStickyFault() {
    return SolenoidJNI.getPCMSolenoidVoltageStickyFault(m_moduleNumber);
  }

  /**
   * The common highside solenoid voltage rail is too low, most likely a solenoid channel is
   * shorted.
   *
   * @return true if PCM is in fault state.
   */
  public boolean getPCMSolenoidVoltageFault() {
    return SolenoidJNI.getPCMSolenoidVoltageFault(m_moduleNumber);
  }

  /**
   * Clear ALL sticky faults inside PCM that Compressor is wired to.
   *
   * <p>If a sticky fault is set, then it will be persistently cleared. Compressor drive maybe
   * momentarily disable while flags are being cleared. Care should be taken to not call this too
   * frequently, otherwise normal compressor functionality may be prevented.
   *
   * <p>If no sticky faults are set then this call will have no effect.
   */
  public void clearAllPCMStickyFaults() {
    SolenoidJNI.clearAllPCMStickyFaults(m_moduleNumber);
  }
}
