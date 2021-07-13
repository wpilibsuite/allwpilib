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
 * Class for operating a compressor connected to a PCM (Pneumatic Control Module). The PCM will
 * automatically run in closed loop mode by default whenever a {@link Solenoid} object is created.
 * For most cases, a Compressor object does not need to be instantiated or used in a robot program.
 * This class is only required in cases where the robot program needs a more detailed status of the
 * compressor or to enable/disable closed loop control.
 *
 * <p>Note: you cannot operate the compressor directly from this class as doing so would circumvent
 * the safety provided by using the pressure switch and closed loop control. You can only turn off
 * closed loop control, thereby stopping the compressor from operating.
 */
public class Compressor implements Sendable, AutoCloseable {
  private PneumaticsBase m_module;

  /**
   * Makes a new instance of the compressor using the provided CAN device ID. Use this constructor
   * when you have more than one PCM.
   *
   * @param module The PCM CAN device ID (0 - 62 inclusive)
   */
  public Compressor(int module, PneumaticsModuleType moduleType) {
    m_module = PneumaticsBase.getForType(module, moduleType);

    if (!m_module.reserveCompressor()) {
      throw new AllocationException("Compressor already allocated");
    }

    m_module.setClosedLoopControl(true);

    HAL.report(tResourceType.kResourceType_Compressor, module + 1);
    SendableRegistry.addLW(this, "Compressor", module);
  }

  /**
   * Makes a new instance of the compressor using the default PCM ID of 0.
   *
   * <p>Additional modules can be supported by making a new instance and specifying the CAN ID.}
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
   */
  public void start() {
    setClosedLoopControl(true);
  }

  /**
   * Stop the compressor from running in closed loop control mode.
   *
   * <p>Use the method in cases where you would like to manually stop and start the compressor for
   * applications such as conserving battery or making sure that the compressor motor doesn't start
   * during critical operations.
   */
  public void stop() {
    setClosedLoopControl(false);
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
  public double getCompressorCurrent() {
    return m_module.getCompressorCurrent();
  }

  /**
   * Set the PCM in closed loop control mode.
   *
   * @param on if true sets the compressor to be in closed loop control mode (default)
   */
  public void setClosedLoopControl(boolean on) {
    m_module.setClosedLoopControl(on);
  }

  /**
   * Gets the current operating mode of the PCM.
   *
   * @return true if compressor is operating on closed-loop mode
   */
  public boolean getClosedLoopControl() {
    return m_module.getClosedLoopControl();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Compressor");
    builder.addBooleanProperty(
        "Closed Loop Control", this::getClosedLoopControl, this::setClosedLoopControl);
    builder.addBooleanProperty("Enabled", this::enabled, null);
    builder.addBooleanProperty("Pressure switch", this::getPressureSwitchValue, null);
  }
}
