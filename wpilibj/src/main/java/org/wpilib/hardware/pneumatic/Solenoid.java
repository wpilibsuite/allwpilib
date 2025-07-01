// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.pneumatic;

import org.wpilib.hardware.hal.util.AllocationException;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * Solenoid class for running high voltage Digital Output on a pneumatics module.
 *
 * <p>The Solenoid class is typically used for pneumatic solenoids, but could be used for any device
 * within the current spec of the module.
 */
public class Solenoid implements TelemetryLoggable, AutoCloseable {
  private final int m_mask; // The channel mask
  private final int m_channel;
  private PneumaticsBase m_module;

  /**
   * Constructs a solenoid for a default module and specified type.
   *
   * @param busId The bus ID
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  public Solenoid(final int busId, final PneumaticsModuleType moduleType, final int channel) {
    this(busId, PneumaticsBase.getDefaultForType(moduleType), moduleType, channel);
  }

  /**
   * Constructs a solenoid for a specified module and type.
   *
   * @param busId The bus ID
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   * @param channel The channel the solenoid is on.
   */
  public Solenoid(
      final int busId, final int module, final PneumaticsModuleType moduleType, final int channel) {
    m_module = PneumaticsBase.getForType(busId, module, moduleType);

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

    m_module.reportUsage("Solenoid[" + channel + "]", "Solenoid");
  }

  @Override
  public void close() {
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
   * Set the pulse duration in the pneumatics module. This is used in conjunction with the
   * startPulse method to allow the pneumatics module to control the timing of a pulse.
   *
   * <p>On the PCM, the timing can be controlled in 0.01 second increments, with a maximum of 2.55
   * seconds.
   *
   * <p>On the PH, the timing can be controlled in 0.001 second increments, with a maximum of 65.534
   * seconds.
   *
   * @param duration The duration of the pulse in seconds.
   * @see #startPulse()
   */
  public void setPulseDuration(double duration) {
    long durationMS = (long) (duration * 1000);
    m_module.setOneShotDuration(m_channel, (int) durationMS);
  }

  /**
   * Trigger the pneumatics module to generate a pulse of the duration set in setPulseDuration.
   *
   * @see #setPulseDuration(double)
   */
  public void startPulse() {
    m_module.fireOneShot(m_channel);
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    table.log("Value", get());
  }

  @Override
  public String getTelemetryType() {
    return "Solenoid";
  }
}
