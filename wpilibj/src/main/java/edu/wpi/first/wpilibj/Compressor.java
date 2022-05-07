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
 * Class for operating a compressor connected to a pneumatics module. The module will automatically
 * run in closed loop mode by default whenever a {@link Solenoid} object is created. For most cases,
 * a Compressor object does not need to be instantiated or used in a robot program. This class is
 * only required in cases where the robot program needs a more detailed status of the compressor or
 * to enable/disable closed loop control.
 *
 * <p>Note: you cannot operate the compressor directly from this class as doing so would circumvent
 * the safety provided by using the pressure switch and closed loop control. You can only turn off
 * closed loop control, thereby stopping the compressor from operating.
 */
public class Compressor implements Sendable, AutoCloseable {
  private PneumaticsBase m_module;

  /**
   * Constructs a compressor for a specified module and type.
   *
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   */
  public Compressor(int module, PneumaticsModuleType moduleType) {
    m_module = PneumaticsBase.getForType(module, moduleType);

    if (!m_module.reserveCompressor()) {
      m_module.close();
      throw new AllocationException("Compressor already allocated");
    }

    m_module.enableCompressorDigital();

    HAL.report(tResourceType.kResourceType_Compressor, module + 1);
    SendableRegistry.addLW(this, "Compressor", module);
  }

  /**
   * Constructs a compressor for a default module and specified type.
   *
   * @param moduleType The module type to use.
   */
  public Compressor(PneumaticsModuleType moduleType) {
    this(PneumaticsBase.getDefaultForType(moduleType), moduleType);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_module.unreserveCompressor();
    m_module.close();
    m_module = null;
  }

  /**
   * Get the status of the compressor. To (re)enable the compressor use enableDigital() or
   * enableAnalog(...).
   *
   * @return true if the compressor is on
   * @deprecated To avoid confusion in thinking this (re)enables the compressor use IsEnabled().
   */
  @Deprecated(since = "2023", forRemoval = true)
  public boolean enabled() {
    return isEnabled();
  }

  /**
   * Returns whether the compressor is active or not.
   *
   * @return true if the compressor is on - otherwise false.
   */
  public boolean isEnabled() {
    return m_module.getCompressor();
  }

  /**
   * Get the pressure switch value.
   *
   * @return true if the pressure is low
   */
  public boolean getPressureSwitchValue() {
    return m_module.getPressureSwitch();
  }

  /**
   * Get the current being used by the compressor.
   *
   * @return current consumed by the compressor in amps
   */
  public double getCurrent() {
    return m_module.getCompressorCurrent();
  }

  /**
   * Query the analog input voltage (on channel 0) (if supported).
   *
   * @return The analog input voltage, in volts
   */
  public double getAnalogVoltage() {
    return m_module.getAnalogVoltage(0);
  }

  /**
   * Query the analog sensor pressure (on channel 0) (if supported). Note this is only for use with
   * the REV Analog Pressure Sensor.
   *
   * @return The analog sensor pressure, in PSI
   */
  public double getPressure() {
    return m_module.getPressure(0);
  }

  /** Disable the compressor. */
  public void disable() {
    m_module.disableCompressor();
  }

  /** Enable compressor closed loop control using digital input. */
  public void enableDigital() {
    m_module.enableCompressorDigital();
  }

  /**
   * Enable compressor closed loop control using analog input. Note this is only for use with the
   * REV Analog Pressure Sensor.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI to enable compressor
   * @param maxPressure The maximum pressure in PSI to disable compressor
   */
  public void enableAnalog(double minPressure, double maxPressure) {
    m_module.enableCompressorAnalog(minPressure, maxPressure);
  }

  /**
   * Enable compressor closed loop control using hybrid input. Note this is only for use with the
   * REV Analog Pressure Sensor.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI to enable compressor
   * @param maxPressure The maximum pressure in PSI to disable compressor
   */
  public void enableHybrid(double minPressure, double maxPressure) {
    m_module.enableCompressorHybrid(minPressure, maxPressure);
  }

  /**
   * Gets the current operating mode of the Compressor.
   *
   * @return true if compressor is operating on closed-loop mode
   */
  public CompressorConfigType getConfigType() {
    return m_module.getCompressorConfigType();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Compressor");
    builder.addBooleanProperty("Enabled", this::isEnabled, null);
    builder.addBooleanProperty("Pressure switch", this::getPressureSwitchValue, null);
  }
}
