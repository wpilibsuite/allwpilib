// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PowerDistributionFaults;
import edu.wpi.first.hal.PowerDistributionJNI;
import edu.wpi.first.hal.PowerDistributionStickyFaults;
import edu.wpi.first.hal.PowerDistributionVersion;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicReferenceArray;

/**
 * Class for getting voltage, current, temperature, power, resistance, and energy from the
 * CTRE Power Distribution Panel (PDP) or REV Power Distribution Hub (PDH) over CAN.
 */
public class PowerDistribution implements Sendable, AutoCloseable {
  private final int m_handle;
  private final int m_module;
  private final boolean m_resistanceEnabled;
  private final ResistanceCalculator m_totalResistanceCalculator;
  private final List<ResistanceCalculator> m_channelResistanceCalculators;
  private final AtomicReference<Double> m_totalResistance = new AtomicReference<>(Double.NaN);
  private final AtomicReferenceArray<Double> m_channelResistances;

  @SuppressWarnings("FieldCanBeLocal")
  private final Notifier m_resistanceLoop;

  public static final int kDefaultModule = PowerDistributionJNI.DEFAULT_MODULE;

  /**
   * Seconds to wait before updating resistance.
   */
  public static final double kUpdatePeriod = 0.025;

  public enum ModuleType {
    kCTRE(PowerDistributionJNI.CTRE_TYPE),
    kRev(PowerDistributionJNI.REV_TYPE);

    public final int value;

    ModuleType(int value) {
      this.value = value;
    }
  }

  /**
   * Constructs a PowerDistribution object. To enable calculating resistance, use
   * {@link PowerDistribution#PowerDistribution(int, ModuleType, ResistanceCalculator)} instead.
   *
   * @param module The CAN ID of the PDP/PDH.
   * @param moduleType Module type (CTRE or REV).
   */
  public PowerDistribution(int module, ModuleType moduleType) {
    this(module, moduleType.value, null);
  }

  /**
   * Constructs a PowerDistribution object. To enable calculating resistance, use
   * {@link PowerDistribution#PowerDistribution(int, ModuleType, ResistanceCalculator)} instead.
   *
   * <p>Detects the connected PDP/PDH using the default CAN ID (0 for CTRE and 1 for REV).
   */
  public PowerDistribution() {
    this(kDefaultModule, PowerDistributionJNI.AUTOMATIC_TYPE, null);
  }

  /**
   * Constructs a PowerDistribution object. To disable calculating resistance, use
   * {@link PowerDistribution#PowerDistribution(int, ModuleType)} instead.
   *
   * @param module The CAN ID of the PDP/PDH.
   * @param moduleType Module type (CTRE or REV).
   * @param resistanceCalculator Object to calculate resistance.
   */
  public PowerDistribution(
      int module, ModuleType moduleType, ResistanceCalculator resistanceCalculator) {
    this(module, moduleType.value, resistanceCalculator);
  }

  /**
   * Common constructor to construct a PowerDistribution object.
   *
   * @param module The CAN ID of the PDP/PDH.
   * @param moduleType Module type as an integer (CTRE or REV).
   * @param resistanceCalculator Object to calculate resistance.
   */
  private PowerDistribution(
      int module, int moduleType, ResistanceCalculator resistanceCalculator) {
    m_handle = PowerDistributionJNI.initialize(module, moduleType);
    m_module = PowerDistributionJNI.getModuleNumber(m_handle);
    m_totalResistanceCalculator = resistanceCalculator;

    m_channelResistances = new AtomicReferenceArray<>(this.getNumChannels());
    m_channelResistanceCalculators = new ArrayList<>();
    m_resistanceEnabled = m_totalResistanceCalculator != null;
    if (m_resistanceEnabled) {
      int numChannels = this.getNumChannels();
      for (int i = 0; i < numChannels; i ++) {
        m_channelResistanceCalculators.add(m_totalResistanceCalculator.copy());
      }
      m_resistanceLoop = new Notifier(this::updateResistances);
      m_resistanceLoop.startPeriodic(PowerDistribution.kUpdatePeriod);
    } else {
      m_resistanceLoop = null;
    }

    HAL.report(tResourceType.kResourceType_PDP, m_module + 1);
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
   * Query the temperature of the PDP/PDH.
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
   * Query the current of all monitored channels.
   *
   * @return The current of all the channels in Amperes
   */
  public double getTotalCurrent() {
    return PowerDistributionJNI.getTotalCurrent(m_handle);
  }

  /**
   * Query the total power drawn from the monitored channels.
   *
   * @return the total power in Watts
   */
  public double getTotalPower() {
    return PowerDistributionJNI.getTotalPower(m_handle);
  }

  /**
   * Query the total energy drawn from the monitored channels.
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

  public PowerDistributionVersion getVersion() {
    return PowerDistributionJNI.getVersion(m_handle);
  }

  public PowerDistributionFaults getFaults() {
    return PowerDistributionJNI.getFaults(m_handle);
  }

  public PowerDistributionStickyFaults getStickyFaults() {
    return PowerDistributionJNI.getStickyFaults(m_handle);
  }

  /**
   * Get the robot's resistance. A {@link ResistanceCalculator} should have been passed in
   * the constructor to enable calculating resistance.
   * @return The robot resistance if a resistance calculator was given, {@code NaN} otherwise.
   */
  public double getTotalResistance() {
    if (m_resistanceEnabled) {
      return m_totalResistance.get();
    } else {
      return Double.NaN;
    }
  }

  /**
   * Get the resistance of a channel. A {@link ResistanceCalculator} should have been passed in
   * the constructor to enable calculating resistance.
   * @param channel The channel to get the resistance for
   * @return The channel's resistance if a resistance calculator was given, {@code NaN} otherwise.
   */
  public double getResistance(int channel) {
    if (m_resistanceEnabled) {
      return m_channelResistances.get(channel);
    } else {
      return Double.NaN;
    }
  }

  /**
   * Update the total resistance and the resistance of each channel.
   */
  private void updateResistances() {
    if (m_resistanceEnabled) {
      double voltage = this.getVoltage();
      m_totalResistance.set(
          m_totalResistanceCalculator.calculate(this.getTotalCurrent(), voltage));
      for (int channel = 0; channel < m_channelResistances.length(); channel ++) {
        ResistanceCalculator calculator = m_channelResistanceCalculators.get(channel);
        double resistance = calculator.calculate(this.getCurrent(channel), voltage);
        m_channelResistances.set(channel, resistance);
      }
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PowerDistribution");
    int numChannels = getNumChannels();
    for (int i = 0; i < numChannels; ++i) {
      final int chan = i;
      builder.addDoubleProperty(
          "Chan" + i, () -> PowerDistributionJNI.getChannelCurrentNoError(m_handle, chan), null);
      if (m_resistanceEnabled) {
        builder.addDoubleProperty(
            "ChanResistance" + i,
            () -> this.getResistance(chan), null);
      }
    }
    builder.addDoubleProperty(
        "Voltage", () -> PowerDistributionJNI.getVoltageNoError(m_handle), null);
    builder.addDoubleProperty(
        "TotalCurrent", () -> PowerDistributionJNI.getTotalCurrent(m_handle), null);
    builder.addBooleanProperty(
        "SwitchableChannel",
        () -> PowerDistributionJNI.getSwitchableChannelNoError(m_handle),
        value -> PowerDistributionJNI.setSwitchableChannel(m_handle, value));
    if (m_resistanceEnabled) {
      builder.addDoubleProperty(
          "TotalResistance",
          this::getTotalResistance, null);
    }
  }
}
