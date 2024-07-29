// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output on the pneumatics
 * module.
 *
 * <p>The DoubleSolenoid class is typically used for pneumatics solenoids that have two positions
 * controlled by two separate channels.
 */
public class DoubleSolenoid implements Sendable, AutoCloseable {
  /** Possible values for a DoubleSolenoid. */
  public enum Value {
    /** Off position. */
    kOff,
    /** Forward position. */
    kForward,
    /** Reverse position. */
    kReverse
  }

  private final int m_forwardMask; // The mask for the forward channel.
  private final int m_reverseMask; // The mask for the reverse channel.
  private final int m_mask; // The channel mask
  private PneumaticsBase m_module;
  private final int m_forwardChannel;
  private final int m_reverseChannel;

  /**
   * Constructs a double solenoid for a default module of a specific module type.
   *
   * @param moduleType The module type to use.
   * @param forwardChannel The forward channel on the module to control.
   * @param reverseChannel The reverse channel on the module to control.
   */
  public DoubleSolenoid(
      final PneumaticsModuleType moduleType, final int forwardChannel, final int reverseChannel) {
    this(PneumaticsBase.getDefaultForType(moduleType), moduleType, forwardChannel, reverseChannel);
  }

  /**
   * Constructs a double solenoid for a specified module of a specific module type.
   *
   * @param module The module of the solenoid module to use.
   * @param moduleType The module type to use.
   * @param forwardChannel The forward channel on the module to control.
   * @param reverseChannel The reverse channel on the module to control.
   */
  @SuppressWarnings({"PMD.UseTryWithResources", "this-escape"})
  public DoubleSolenoid(
      final int module,
      final PneumaticsModuleType moduleType,
      final int forwardChannel,
      final int reverseChannel) {
    m_module = PneumaticsBase.getForType(module, moduleType);
    boolean allocatedSolenoids = false;
    boolean successfulCompletion = false;

    m_forwardChannel = forwardChannel;
    m_reverseChannel = reverseChannel;

    m_forwardMask = 1 << forwardChannel;
    m_reverseMask = 1 << reverseChannel;
    m_mask = m_forwardMask | m_reverseMask;

    try {
      if (!m_module.checkSolenoidChannel(forwardChannel)) {
        throw new IllegalArgumentException("Channel " + forwardChannel + " out of range");
      }

      if (!m_module.checkSolenoidChannel(reverseChannel)) {
        throw new IllegalArgumentException("Channel " + reverseChannel + " out of range");
      }

      int allocMask = m_module.checkAndReserveSolenoids(m_mask);
      if (allocMask != 0) {
        if (allocMask == m_mask) {
          throw new AllocationException(
              "Channels " + forwardChannel + " and " + reverseChannel + " already allocated");
        } else if (allocMask == m_forwardMask) {
          throw new AllocationException("Channel " + forwardChannel + " already allocated");
        } else {
          throw new AllocationException("Channel " + reverseChannel + " already allocated");
        }
      }
      allocatedSolenoids = true;

      HAL.report(
          tResourceType.kResourceType_Solenoid, forwardChannel + 1, m_module.getModuleNumber() + 1);
      HAL.report(
          tResourceType.kResourceType_Solenoid, reverseChannel + 1, m_module.getModuleNumber() + 1);
      SendableRegistry.addLW(this, "DoubleSolenoid", m_module.getModuleNumber(), forwardChannel);
      successfulCompletion = true;
    } finally {
      if (!successfulCompletion) {
        if (allocatedSolenoids) {
          m_module.unreserveSolenoids(m_mask);
        }
        m_module.close();
      }
    }
  }

  @Override
  public synchronized void close() {
    SendableRegistry.remove(this);
    m_module.unreserveSolenoids(m_mask);
    m_module.close();
    m_module = null;
  }

  /**
   * Set the value of a solenoid.
   *
   * @param value The value to set (Off, Forward, Reverse)
   */
  public void set(final Value value) {
    int setValue =
        switch (value) {
          case kOff -> 0;
          case kForward -> m_forwardMask;
          case kReverse -> m_reverseMask;
        };

    m_module.setSolenoids(m_mask, setValue);
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  public Value get() {
    int values = m_module.getSolenoids();

    if ((values & m_forwardMask) != 0) {
      return Value.kForward;
    } else if ((values & m_reverseMask) != 0) {
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
   * Get the forward channel.
   *
   * @return the forward channel.
   */
  public int getFwdChannel() {
    return m_forwardChannel;
  }

  /**
   * Get the reverse channel.
   *
   * @return the reverse channel.
   */
  public int getRevChannel() {
    return m_reverseChannel;
  }

  /**
   * Check if the forward solenoid is Disabled. If a solenoid is shorted, it is added to the
   * DisabledList and disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   */
  public boolean isFwdSolenoidDisabled() {
    return (m_module.getSolenoidDisabledList() & m_forwardMask) != 0;
  }

  /**
   * Check if the reverse solenoid is Disabled. If a solenoid is shorted, it is added to the
   * DisabledList and disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   */
  public boolean isRevSolenoidDisabled() {
    return (m_module.getSolenoidDisabledList() & m_reverseMask) != 0;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Double Solenoid");
    builder.setActuator(true);
    builder.setSafeState(() -> set(Value.kOff));
    builder.addStringProperty(
        "Value",
        () -> get().name().substring(1),
        value -> {
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
