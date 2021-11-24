// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PowerDistributionJNI;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class for getting voltage, current, temperature, power and energy from the Power Distribution
 * Panel over CAN.
 */
public class PowerDistribution implements Sendable, AutoCloseable {
  private final int m_handle;
  private final int m_module;

  public static final int kDefaultModule = PowerDistributionJNI.DEFAULT_MODULE;

  public enum ModuleType {
    kCTRE(PowerDistributionJNI.CTRE_TYPE),
    kRev(PowerDistributionJNI.REV_TYPE);

    public final int value;

    ModuleType(int value) {
      this.value = value;
    }
  }

  /**
   * Constructs a PowerDistribution.
   *
   * @param module The CAN ID of the PDP.
   * @param moduleType Module type (CTRE or REV).
   */
  public PowerDistribution(int module, ModuleType moduleType) {
    m_handle = PowerDistributionJNI.initialize(module, moduleType.value);
    m_module = PowerDistributionJNI.getModuleNumber(m_handle);

    HAL.report(tResourceType.kResourceType_PDP, m_module + 1);
    SendableRegistry.addLW(this, "PowerDistribution", m_module);
  }

  /**
   * Constructs a PowerDistribution.
   *
   * <p>Uses the default CAN ID (0 for CTRE and 1 for REV).
   */
  public PowerDistribution() {
    m_handle = PowerDistributionJNI.initialize(kDefaultModule, PowerDistributionJNI.AUTOMATIC_TYPE);
    m_module = PowerDistributionJNI.getModuleNumber(m_handle);

    HAL.report(tResourceType.kResourceType_PDP, m_module + 1);
    SendableRegistry.addLW(this, "PowerDistribution", m_module);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Gets the number of channel for this power distribution.
   *
   * @return Number of output channels.
   */
  public int getNumChannels() {
    return PowerDistributionJNI.getNumChannels(m_handle);
  }

  /**
   * Query the input voltage of the PDP.
   *
   * @return The voltage of the PDP in volts
   */
  public double getVoltage() {
    return PowerDistributionJNI.getVoltage(m_handle);
  }

  /**
   * Query the temperature of the PDP.
   *
   * @return The temperature of the PDP in degrees Celsius
   */
  public double getTemperature() {
    return PowerDistributionJNI.getTemperature(m_handle);
  }

  /**
   * Query the current of a single channel of the PDP.
   *
   * @param channel The PDP channel to query.
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   */
  public double getCurrent(int channel) {
    double current = PowerDistributionJNI.getChannelCurrent(m_handle, channel);

    return current;
  }

  /**
   * Query the current of all monitored PDP channels (0-15).
   *
   * @return The current of all the channels in Amperes
   */
  public double getTotalCurrent() {
    return PowerDistributionJNI.getTotalCurrent(m_handle);
  }

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @return the total power in Watts
   */
  public double getTotalPower() {
    return PowerDistributionJNI.getTotalPower(m_handle);
  }

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @return the total energy in Joules
   */
  public double getTotalEnergy() {
    return PowerDistributionJNI.getTotalEnergy(m_handle);
  }

  /** Reset the total energy to 0. */
  public void resetTotalEnergy() {
    PowerDistributionJNI.resetTotalEnergy(m_handle);
  }

  /** Clear all PDP sticky faults. */
  public void clearStickyFaults() {
    PowerDistributionJNI.clearStickyFaults(m_handle);
  }

  /**
   * Gets module number (CAN ID).
   *
   * @return The module number (CAN ID).
   */
  public int getModule() {
    return m_module;
  }

  public boolean getSwitchableChannel() {
    return PowerDistributionJNI.getSwitchableChannel(m_handle);
  }

  public void setSwitchableChannel(boolean enabled) {
    PowerDistributionJNI.setSwitchableChannel(m_handle, enabled);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PowerDistribution");
    int numChannels = getNumChannels();
    for (int i = 0; i < numChannels; ++i) {
      final int chan = i;
      builder.addDoubleProperty(
          "Chan" + i, () -> PowerDistributionJNI.getChannelCurrentNoError(m_handle, chan), null);
    }
    builder.addDoubleProperty(
        "Voltage", () -> PowerDistributionJNI.getVoltageNoError(m_handle), null);
    builder.addDoubleProperty(
        "TotalCurrent", () -> PowerDistributionJNI.getTotalCurrent(m_handle), null);
    builder.addBooleanProperty(
        "SwitchableChannel",
        () -> PowerDistributionJNI.getSwitchableChannelNoError(m_handle),
        value -> PowerDistributionJNI.setSwitchableChannel(m_handle, value));
  }
}
