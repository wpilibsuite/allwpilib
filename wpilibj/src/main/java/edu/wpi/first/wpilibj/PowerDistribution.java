// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PowerDistributionFaults;
import edu.wpi.first.hal.PowerDistributionJNI;
import edu.wpi.first.hal.PowerDistributionStickyFaults;
import edu.wpi.first.hal.PowerDistributionVersion;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class for getting voltage, current, temperature, power and energy from the CTRE Power
 * Distribution Panel (PDP) or REV Power Distribution Hub (PDH) over CAN.
 */
public class PowerDistribution implements Sendable, AutoCloseable {
  private final int m_handle;
  private final int m_module;

  /** Default module number. */
  public static final int kDefaultModule = PowerDistributionJNI.DEFAULT_MODULE;

  /** Power distribution module type. */
  public enum ModuleType {
    /** CTRE (Cross The Road Electronics) Power Distribution Panel (PDP). */
    kCTRE(PowerDistributionJNI.CTRE_TYPE),
    /** REV Power Distribution Hub (PDH). */
    kRev(PowerDistributionJNI.REV_TYPE);

    /** ModuleType value. */
    public final int value;

    ModuleType(int value) {
      this.value = value;
    }
  }

  /**
   * Constructs a PowerDistribution object.
   *
   * @param module The CAN ID of the PDP/PDH.
   * @param moduleType Module type (CTRE or REV).
   */
  @SuppressWarnings("this-escape")
  public PowerDistribution(int module, ModuleType moduleType) {
    m_handle = PowerDistributionJNI.initialize(module, moduleType.value);
    m_module = PowerDistributionJNI.getModuleNumber(m_handle);

    if (moduleType == ModuleType.kCTRE) {
      HAL.report(tResourceType.kResourceType_PDP, tInstances.kPDP_CTRE);
    } else {
      HAL.report(tResourceType.kResourceType_PDP, tInstances.kPDP_REV);
    }
    SendableRegistry.addLW(this, "PowerDistribution", m_module);
  }

  /**
   * Constructs a PowerDistribution object.
   *
   * <p>Detects the connected PDP/PDH using the default CAN ID (0 for CTRE and 1 for REV).
   */
  @SuppressWarnings("this-escape")
  public PowerDistribution() {
    m_handle = PowerDistributionJNI.initialize(kDefaultModule, PowerDistributionJNI.AUTOMATIC_TYPE);
    m_module = PowerDistributionJNI.getModuleNumber(m_handle);

    if (PowerDistributionJNI.getType(m_handle) == PowerDistributionJNI.CTRE_TYPE) {
      HAL.report(tResourceType.kResourceType_PDP, tInstances.kPDP_CTRE);
    } else {
      HAL.report(tResourceType.kResourceType_PDP, tInstances.kPDP_REV);
    }

    SendableRegistry.addLW(this, "PowerDistribution", m_module);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Gets the number of channels for this power distribution object.
   *
   * @return Number of output channels (16 for PDP, 24 for PDH).
   */
  public int getNumChannels() {
    return PowerDistributionJNI.getNumChannels(m_handle);
  }

  /**
   * Query the input voltage of the PDP/PDH.
   *
   * @return The voltage in volts
   */
  public double getVoltage() {
    return PowerDistributionJNI.getVoltage(m_handle);
  }

  /**
   * Query the temperature of the PDP.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @return The temperature in degrees Celsius
   */
  public double getTemperature() {
    return PowerDistributionJNI.getTemperature(m_handle);
  }

  /**
   * Query the current of a single channel of the PDP/PDH.
   *
   * @param channel The channel (0-15 for PDP, 0-23 for PDH) to query
   * @return The current of the channel in Amperes
   */
  public double getCurrent(int channel) {
    return PowerDistributionJNI.getChannelCurrent(m_handle, channel);
  }

  /**
   * Query all currents of the PDP.
   *
   * @return The current of each channel in Amperes
   */
  public double[] getAllCurrents() {
    double[] currents = new double[getNumChannels()];
    PowerDistributionJNI.getAllCurrents(m_handle, currents);
    return currents;
  }

  /**
   * Query the current of all monitored channels.
   *
   * @return The current of all the channels in Amperes
   */
  public double getTotalCurrent() {
    return PowerDistributionJNI.getTotalCurrent(m_handle);
  }

  /**
   * Query the total power drawn from the monitored channels of the PDP.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @return the total power in Watts
   */
  public double getTotalPower() {
    return PowerDistributionJNI.getTotalPower(m_handle);
  }

  /**
   * Query the total energy drawn from the monitored channels of the PDP.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @return the total energy in Joules
   */
  public double getTotalEnergy() {
    return PowerDistributionJNI.getTotalEnergy(m_handle);
  }

  /**
   * Reset the total energy to 0 of the PDP.
   *
   * <p>Not supported on the Rev PDH and does nothing.
   */
  public void resetTotalEnergy() {
    PowerDistributionJNI.resetTotalEnergy(m_handle);
  }

  /** Clear all PDP/PDH sticky faults. */
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

  /**
   * Gets the module type for this power distribution object.
   *
   * @return The module type
   */
  public ModuleType getType() {
    int type = PowerDistributionJNI.getType(m_handle);
    if (type == PowerDistributionJNI.REV_TYPE) {
      return ModuleType.kRev;
    } else {
      return ModuleType.kCTRE;
    }
  }

  /**
   * Gets whether the PDH switchable channel is turned on or off. Returns false with the CTRE PDP.
   *
   * @return The output state of the PDH switchable channel
   */
  public boolean getSwitchableChannel() {
    return PowerDistributionJNI.getSwitchableChannel(m_handle);
  }

  /**
   * Sets the PDH switchable channel on or off. Does nothing with the CTRE PDP.
   *
   * @param enabled Whether to turn the PDH switchable channel on or off
   */
  public void setSwitchableChannel(boolean enabled) {
    PowerDistributionJNI.setSwitchableChannel(m_handle, enabled);
  }

  /**
   * Returns the power distribution version number.
   *
   * @return The power distribution version number.
   */
  public PowerDistributionVersion getVersion() {
    return PowerDistributionJNI.getVersion(m_handle);
  }

  /**
   * Returns the power distribution faults.
   *
   * <p>On a CTRE PDP, this will return an object with no faults active.
   *
   * @return The power distribution faults.
   */
  public PowerDistributionFaults getFaults() {
    return PowerDistributionJNI.getFaults(m_handle);
  }

  /**
   * Returns the power distribution sticky faults.
   *
   * <p>On a CTRE PDP, this will return an object with no faults active.
   *
   * @return The power distribution sticky faults.
   */
  public PowerDistributionStickyFaults getStickyFaults() {
    return PowerDistributionJNI.getStickyFaults(m_handle);
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
