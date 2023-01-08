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
 * Solenoid class for running high voltage Digital Output on a pneumatics module.
 *
 * <p>The Solenoid class is typically used for pneumatic solenoids, but could be used for any device
 * within the current spec of the module.
 */
public class Solenoid implements Sendable, AutoCloseable {
  private final int m_mask; // The channel mask
  private final int m_channel;
  private PneumaticsBase m_module;

  /**
   * Constructs a solenoid for a default module and specified type.
   *
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  public Solenoid(final PneumaticsModuleType moduleType, final int channel) {
    this(PneumaticsBase.getDefaultForType(moduleType), moduleType, channel);
  }

  /**
   * Constructs a solenoid for a specified module and type.
   *
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  public Solenoid(final int module, final PneumaticsModuleType moduleType, final int channel) {
    m_module = PneumaticsBase.getForType(module, moduleType);

    m_mask = 1 << channel;
    m_channel = channel;

    if (!m_module.checkSolenoidChannel(channel)) {
      m_module.close();
      throw new IllegalArgumentException("Channel " + channel + " out of range");
    }

    if (m_module.checkAndReserveSolenoids(m_mask) != 0) {
      m_module.close();
      throw new AllocationException("Solenoid already allocated");
    }

    HAL.report(tResourceType.kResourceType_Solenoid, channel + 1, m_module.getModuleNumber() + 1);
    SendableRegistry.addLW(this, "Solenoid", m_module.getModuleNumber(), channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_module.unreserveSolenoids(m_mask);
    m_module.close();
    m_module = null;
  }

  /**
   * Set the value of a solenoid.
   *
   * @param on True will turn the solenoid output on. False will turn the solenoid output off.
   */
  public void set(boolean on) {
    int value = on ? (0xFFFF & m_mask) : 0;
    m_module.setSolenoids(m_mask, value);
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return True if the solenoid output is on or false if the solenoid output is off.
   */
  public boolean get() {
    int currentAll = m_module.getSolenoids();
    return (currentAll & m_mask) != 0;
  }

  /**
   * Toggle the value of the solenoid.
   *
   * <p>If the solenoid is set to on, it'll be turned off. If the solenoid is set to off, it'll be
   * turned on.
   */
  public void toggle() {
    set(!get());
  }

  /**
   * Get the channel this solenoid is connected to.
   *
   * @return The channel this solenoid is connected to.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Check if solenoid is DisabledListed. If a solenoid is shorted, it is added to the Disabled List
   * and disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   */
  public boolean isDisabled() {
    return (m_module.getSolenoidDisabledList() & m_mask) != 0;
  }

  /**
   * Set the pulse duration in the PCM. This is used in conjunction with the startPulse method to
   * allow the PCM to control the timing of a pulse. The timing can be controlled in 0.01 second
   * increments.
   *
   * @param durationSeconds The duration of the pulse, from 0.01 to 2.55 seconds.
   * @see #startPulse()
   */
  public void setPulseDuration(double durationSeconds) {
    long durationMS = (long) (durationSeconds * 1000);
    m_module.setOneShotDuration(m_channel, (int) durationMS);
  }

  /**
   * Trigger the PCM to generate a pulse of the duration set in setPulseDuration.
   *
   * @see #setPulseDuration(double)
   */
  public void startPulse() {
    m_module.fireOneShot(m_channel);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Solenoid");
    builder.setActuator(true);
    builder.setSafeState(() -> set(false));
    builder.addBooleanProperty("Value", this::get, this::set);
  }
}
