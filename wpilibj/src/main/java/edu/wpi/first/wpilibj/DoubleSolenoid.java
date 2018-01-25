/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.hal.SolenoidJNI;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output on the PCM.
 *
 * <p>The DoubleSolenoid class is typically used for pneumatics solenoids that have two positions
 * controlled by two separate channels.
 */
public class DoubleSolenoid extends SolenoidBase implements Sendable {
  /**
   * Possible values for a DoubleSolenoid.
   */
  public enum Value {
    kOff,
    kForward,
    kReverse
  }

  private byte m_forwardMask; // The mask for the forward channel.
  private byte m_reverseMask; // The mask for the reverse channel.
  private int m_forwardHandle = 0;
  private int m_reverseHandle = 0;

  /**
   * Constructor. Uses the default PCM ID (defaults to 0).
   *
   * @param forwardChannel The forward channel number on the PCM (0..7).
   * @param reverseChannel The reverse channel number on the PCM (0..7).
   */
  public DoubleSolenoid(final int forwardChannel, final int reverseChannel) {
    this(SensorBase.getDefaultSolenoidModule(), forwardChannel, reverseChannel);
  }

  /**
   * Constructor.
   *
   * @param moduleNumber   The module number of the solenoid module to use.
   * @param forwardChannel The forward channel on the module to control (0..7).
   * @param reverseChannel The reverse channel on the module to control (0..7).
   */
  public DoubleSolenoid(final int moduleNumber, final int forwardChannel,
                        final int reverseChannel) {
    super(moduleNumber);

    SensorBase.checkSolenoidModule(m_moduleNumber);
    SensorBase.checkSolenoidChannel(forwardChannel);
    SensorBase.checkSolenoidChannel(reverseChannel);

    int portHandle = SolenoidJNI.getPortWithModule((byte) m_moduleNumber, (byte) forwardChannel);
    m_forwardHandle = SolenoidJNI.initializeSolenoidPort(portHandle);

    try {
      portHandle = SolenoidJNI.getPortWithModule((byte) m_moduleNumber, (byte) reverseChannel);
      m_reverseHandle = SolenoidJNI.initializeSolenoidPort(portHandle);
    } catch (RuntimeException ex) {
      // free the forward handle on exception, then rethrow
      SolenoidJNI.freeSolenoidPort(m_forwardHandle);
      m_forwardHandle = 0;
      m_reverseHandle = 0;
      throw ex;
    }

    m_forwardMask = (byte) (1 << forwardChannel);
    m_reverseMask = (byte) (1 << reverseChannel);

    HAL.report(tResourceType.kResourceType_Solenoid, forwardChannel,
                                   m_moduleNumber);
    HAL.report(tResourceType.kResourceType_Solenoid, reverseChannel,
                                   m_moduleNumber);
    setName("DoubleSolenoid", m_moduleNumber, forwardChannel);
  }

  /**
   * Destructor.
   */
  @Override
  public synchronized void free() {
    super.free();
    SolenoidJNI.freeSolenoidPort(m_forwardHandle);
    SolenoidJNI.freeSolenoidPort(m_reverseHandle);
    super.free();
  }

  /**
   * Set the value of a solenoid.
   *
   * @param value The value to set (Off, Forward, Reverse)
   */
  public void set(final Value value) {
    boolean forward = false;
    boolean reverse = false;

    switch (value) {
      case kOff:
        forward = false;
        reverse = false;
        break;
      case kForward:
        forward = true;
        reverse = false;
        break;
      case kReverse:
        forward = false;
        reverse = true;
        break;
      default:
        throw new AssertionError("Illegal value: " + value);

    }

    SolenoidJNI.setSolenoid(m_forwardHandle, forward);
    SolenoidJNI.setSolenoid(m_reverseHandle, reverse);
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  public Value get() {
    boolean valueForward = SolenoidJNI.getSolenoid(m_forwardHandle);
    boolean valueReverse = SolenoidJNI.getSolenoid(m_reverseHandle);

    if (valueForward) {
      return Value.kForward;
    }
    if (valueReverse) {
      return Value.kReverse;
    }
    return Value.kOff;
  }

  /**
   * Check if the forward solenoid is blacklisted. If a solenoid is shorted, it is added to the
   * blacklist and disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   * @see #clearAllPCMStickyFaults()
   */
  public boolean isFwdSolenoidBlackListed() {
    int blackList = getPCMSolenoidBlackList();
    return (blackList & m_forwardMask) != 0;
  }

  /**
   * Check if the reverse solenoid is blacklisted. If a solenoid is shorted, it is added to the
   * blacklist and disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   * @see #clearAllPCMStickyFaults()
   */
  public boolean isRevSolenoidBlackListed() {
    int blackList = getPCMSolenoidBlackList();
    return (blackList & m_reverseMask) != 0;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Double Solenoid");
    builder.setSafeState(() -> set(Value.kOff));
    builder.addStringProperty("Value", () -> get().name().substring(1), (value) -> {
      if ("Forward".equals(value)) {
        set(Value.kForward);
      } else if ("Reverse".equals(value)) {
        set(Value.kReverse);
      } else {
        set(Value.kOff);
      }
    });
  }
}
