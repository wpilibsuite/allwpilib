/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SolenoidJNI;
import edu.wpi.first.hal.util.UncleanStatusException;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output on the PCM.
 *
 * <p>The DoubleSolenoid class is typically used for pneumatics solenoids that have two positions
 * controlled by two separate channels.
 */
public class DoubleSolenoid extends SolenoidBase implements Sendable, AutoCloseable {
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
  private int m_forwardHandle;
  private int m_reverseHandle;

  /**
   * Constructor. Uses the default PCM ID (defaults to 0).
   *
   * @param forwardChannel The forward channel number on the PCM (0..7).
   * @param reverseChannel The reverse channel number on the PCM (0..7).
   */
  public DoubleSolenoid(final int forwardChannel, final int reverseChannel) {
    this(SensorUtil.getDefaultSolenoidModule(), forwardChannel, reverseChannel);
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

    SensorUtil.checkSolenoidModule(m_moduleNumber);
    SensorUtil.checkSolenoidChannel(forwardChannel);
    SensorUtil.checkSolenoidChannel(reverseChannel);

    int portHandle = HAL.getPortWithModule((byte) m_moduleNumber, (byte) forwardChannel);
    m_forwardHandle = SolenoidJNI.initializeSolenoidPort(portHandle);

    try {
      portHandle = HAL.getPortWithModule((byte) m_moduleNumber, (byte) reverseChannel);
      m_reverseHandle = SolenoidJNI.initializeSolenoidPort(portHandle);
    } catch (UncleanStatusException ex) {
      // free the forward handle on exception, then rethrow
      SolenoidJNI.freeSolenoidPort(m_forwardHandle);
      m_forwardHandle = 0;
      m_reverseHandle = 0;
      throw ex;
    }

    m_forwardMask = (byte) (1 << forwardChannel);
    m_reverseMask = (byte) (1 << reverseChannel);

    HAL.report(tResourceType.kResourceType_Solenoid, forwardChannel + 1,
                                   m_moduleNumber + 1);
    HAL.report(tResourceType.kResourceType_Solenoid, reverseChannel + 1,
                                   m_moduleNumber + 1);
    SendableRegistry.addLW(this, "DoubleSolenoid", m_moduleNumber, forwardChannel);
  }

  @Override
  public synchronized void close() {
    SendableRegistry.remove(this);
    SolenoidJNI.freeSolenoidPort(m_forwardHandle);
    SolenoidJNI.freeSolenoidPort(m_reverseHandle);
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
    } else if (valueReverse) {
      return Value.kReverse;
    } else {
      return Value.kOff;
    }
  }

  /**
   * Toggle the value of the solenoid.
   *
   * <p>If the solenoid is set to forward, it'll be set to reverse. If the solenoid is set to
   * reverse, it'll be set to forward. If the solenoid is set to off, nothing happens.
   */
  public void toggle() {
    Value value = get();

    if (value == Value.kForward) {
      set(Value.kReverse);
    } else if (value == Value.kReverse) {
      set(Value.kForward);
    }
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
    builder.setActuator(true);
    builder.setSafeState(() -> set(Value.kOff));
    builder.addStringProperty("Value", () -> get().name().substring(1), value -> {
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
