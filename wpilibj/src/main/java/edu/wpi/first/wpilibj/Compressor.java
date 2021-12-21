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
   * Start the compressor running in closed loop control mode.
   *
   * <p>Use the method in cases where you would like to manually stop and start the compressor for
   * applications such as conserving battery or making sure that the compressor motor doesn't start
   * during critical operations.
   *
   * @deprecated Use enableDigital() instead.
   */
  @Deprecated(since = "2022", forRemoval = true)
  public void start() {
    enableDigital();
  }

  /**
   * Stop the compressor from running in closed loop control mode.
   *
   * <p>Use the method in cases where you would like to manually stop and start the compressor for
   * applications such as conserving battery or making sure that the compressor motor doesn't start
   * during critical operations.
   *
   * @deprecated Use disable() instead.
   */
  @Deprecated(since = "2022", forRemoval = true)
  public void stop() {
    disable();
  }

  /**
   * Get the status of the compressor.
   *
   * @return true if the compressor is on
   */
  public boolean enabled() {
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

  /** Disable the compressor. */
  public void disable() {
    m_module.disableCompressor();
  }

  /** Enable compressor closed loop control using digital input. */
  public void enableDigital() {
    m_module.enableCompressorDigital();
  }

  /**
   * Enable compressor closed loop control using analog input.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minAnalogVoltage The minimum voltage to enable compressor
   * @param maxAnalogVoltage The maximum voltage to disable compressor
   */
  public void enableAnalog(double minAnalogVoltage, double maxAnalogVoltage) {
    m_module.enableCompressorAnalog(minAnalogVoltage, maxAnalogVoltage);
  }

  /**
   * Enable compressor closed loop control using hybrid input.
   *
   * <p>On CTRE PCM, this will enable digital control.
   *
   * @param minAnalogVoltage The minimum voltage to enable compressor
   * @param maxAnalogVoltage The maximum voltage to disable compressor
   */
  public void enableHybrid(double minAnalogVoltage, double maxAnalogVoltage) {
    m_module.enableCompressorHybrid(minAnalogVoltage, maxAnalogVoltage);
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
    builder.addBooleanProperty("Enabled", this::enabled, null);
    builder.addBooleanProperty("Pressure switch", this::getPressureSwitchValue, null);
  }
}
