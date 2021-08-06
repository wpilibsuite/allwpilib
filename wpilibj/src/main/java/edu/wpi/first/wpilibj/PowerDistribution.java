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

  /**
   * Constructs a PowerDistribution.
   *
   * @param module The CAN ID of the PDP
   */
  public PowerDistribution(int module) {
    m_handle = PowerDistributionJNI.initialize(module, 0);
    m_module = module;

    HAL.report(tResourceType.kResourceType_PDP, module + 1);
    SendableRegistry.addLW(this, "PowerDistribution", module);
  }

  /**
   * Constructs a PowerDistribution.
   *
   * <p>Uses the default CAN ID (0).
   */
  public PowerDistribution() {
    this(0);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
    double current = PowerDistributionJNI.getChannelCurrent((byte) channel, m_handle);

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

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PowerDistribution");
    for (int i = 0; i < SensorUtil.kPDPChannels; ++i) {
      final int chan = i;
      builder.addDoubleProperty("Chan" + i, () -> getCurrent(chan), null);
    }
    builder.addDoubleProperty("Voltage", this::getVoltage, null);
    builder.addDoubleProperty("TotalCurrent", this::getTotalCurrent, null);
  }
}
