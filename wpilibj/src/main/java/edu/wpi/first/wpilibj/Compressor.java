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
  private PneumaticsModuleType m_moduleType;

  /**
   * Constructs a compressor for a specified module and type.
   *
   * @param module The module ID to use.
   * @param moduleType The module type to use.
   */
  @SuppressWarnings("this-escape")
  public Compressor(int module, PneumaticsModuleType moduleType) {
    m_module = PneumaticsBase.getForType(module, moduleType);
    m_moduleType = moduleType;

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
   * Returns whether the compressor is active or not.
   *
   * @return true if the compressor is on - otherwise false.
   */
  public boolean isEnabled() {
    return m_module.getCompressor();
  }

  /**
   * Returns the state of the pressure switch.
   *
   * @return True if pressure switch indicates that the system is not full, otherwise false.
   */
  public boolean getPressureSwitchValue() {
    return m_module.getPressureSwitch();
  }

  /**
   * Get the current drawn by the compressor.
   *
   * @return Current drawn by the compressor in amps.
   */
  public double getCurrent() {
    return m_module.getCompressorCurrent();
  }

  /**
   * If supported by the device, returns the analog input voltage (on channel 0).
   *
   * <p>This function is only supported by the REV PH. On CTRE PCM, this will return 0.
   *
   * @return The analog input voltage, in volts.
   */
  public double getAnalogVoltage() {
    return m_module.getAnalogVoltage(0);
  }

  /**
   * If supported by the device, returns the pressure (in PSI) read by the analog pressure sensor
   * (on channel 0).
   *
   * <p>This function is only supported by the REV PH with the REV Analog Pressure Sensor. On CTRE
   * PCM, this will return 0.
   *
   * @return The pressure (in PSI) read by the analog pressure sensor.
   */
  public double getPressure() {
    return m_module.getPressure(0);
  }

  /** Disable the compressor. */
  public void disable() {
    m_module.disableCompressor();
  }

  /**
   * Enables the compressor in digital mode using the digital pressure switch. The compressor will
   * turn on when the pressure switch indicates that the system is not full, and will turn off when
   * the pressure switch indicates that the system is full.
   */
  public void enableDigital() {
    m_module.enableCompressorDigital();
  }

  /**
   * If supported by the device, enables the compressor in analog mode. This mode uses an analog
   * pressure sensor connected to analog channel 0 to cycle the compressor. The compressor will turn
   * on when the pressure drops below {@code minPressure} and will turn off when the pressure
   * reaches {@code maxPressure}. This mode is only supported by the REV PH with the REV Analog
   * Pressure Sensor connected to analog channel 0.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value.
   */
  public void enableAnalog(double minPressure, double maxPressure) {
    m_module.enableCompressorAnalog(minPressure, maxPressure);
  }

  /**
   * If supported by the device, enables the compressor in hybrid mode. This mode uses both a
   * digital pressure switch and an analog pressure sensor connected to analog channel 0 to cycle
   * the compressor. This mode is only supported by the REV PH with the REV Analog Pressure Sensor
   * connected to analog channel 0.
   *
   * <p>The compressor will turn on when <i>both</i>:
   *
   * <ul>
   *   <li>The digital pressure switch indicates the system is not full AND
   *   <li>The analog pressure sensor indicates that the pressure in the system is below the
   *       specified minimum pressure.
   * </ul>
   *
   * <p>The compressor will turn off when <i>either</i>:
   *
   * <ul>
   *   <li>The digital pressure switch is disconnected or indicates that the system is full OR
   *   <li>The pressure detected by the analog sensor is greater than the specified maximum
   *       pressure.
   * </ul>
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value and the pressure switch indicates that the system is not full.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value or the pressure switch is disconnected or indicates that the system is
   *     full.
   */
  public void enableHybrid(double minPressure, double maxPressure) {
    m_module.enableCompressorHybrid(minPressure, maxPressure);
  }

  /**
   * Returns the active compressor configuration.
   *
   * @return The active compressor configuration.
   */
  public CompressorConfigType getConfigType() {
    return m_module.getCompressorConfigType();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Compressor");
    builder.addBooleanProperty("Enabled", this::isEnabled, null);
    builder.addBooleanProperty("Pressure switch", this::getPressureSwitchValue, null);
    builder.addDoubleProperty("Current (A)", this::getCurrent, null);
    if (m_moduleType == PneumaticsModuleType.REVPH) { // These are not supported by the CTRE PCM
      builder.addDoubleProperty("Analog Voltage", this::getAnalogVoltage, null);
      builder.addDoubleProperty("Pressure (PSI)", this::getPressure, null);
    }
  }
}
