// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.PortsJNI;
import edu.wpi.first.hal.REVPHFaults;
import edu.wpi.first.hal.REVPHJNI;
import edu.wpi.first.hal.REVPHStickyFaults;
import edu.wpi.first.hal.REVPHVersion;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.Map;

/** Module class for controlling a REV Robotics Pneumatic Hub. */
public class PneumaticHub implements PneumaticsBase {
  private static class DataStore implements AutoCloseable {
    public final int m_module;
    public final int m_handle;
    private int m_refCount;
    private int m_reservedMask;
    private boolean m_compressorReserved;
    public final int[] m_oneShotDurMs = new int[PortsJNI.getNumREVPHChannels()];
    private final Object m_reserveLock = new Object();

    DataStore(int module) {
      m_handle = REVPHJNI.initialize(module);
      m_module = module;
      m_handleMap.put(module, this);

      final REVPHVersion version = REVPHJNI.getVersion(m_handle);
      final String fwVersion =
          version.firmwareMajor + "." + version.firmwareMinor + "." + version.firmwareFix;

      if (version.firmwareMajor > 0 && RobotBase.isReal()) {
        // Write PH firmware version to roboRIO
        final String fileName = "REV_PH_" + String.format("%02d", module) + "_WPILib_Version.ini";
        final File file = new File("/tmp/frc_versions/" + fileName);
        try {
          if (file.exists() && !file.delete()) {
            throw new IOException("Failed to delete " + fileName);
          }

          if (!file.createNewFile()) {
            throw new IOException("Failed to create new " + fileName);
          }

          try (OutputStream output = Files.newOutputStream(file.toPath())) {
            output.write("[Version]\n".getBytes(StandardCharsets.UTF_8));
            output.write("model=REV PH\n".getBytes(StandardCharsets.UTF_8));
            output.write(
                ("deviceID=" + Integer.toHexString(0x9052600 | module) + "\n")
                    .getBytes(StandardCharsets.UTF_8));
            output.write(("currentVersion=" + fwVersion).getBytes(StandardCharsets.UTF_8));
          }
        } catch (IOException ex) {
          DriverStation.reportError("Could not write " + fileName + ": " + ex, ex.getStackTrace());
        }
      }

      // Check PH firmware version
      if (version.firmwareMajor > 0 && version.firmwareMajor < 22) {
        throw new IllegalStateException(
            "The Pneumatic Hub has firmware version "
                + fwVersion
                + ", and must be updated to version 2022.0.0 or later "
                + "using the REV Hardware Client.");
      }
    }

    @Override
    public void close() {
      REVPHJNI.free(m_handle);
      m_handleMap.remove(m_module);
    }

    public void addRef() {
      m_refCount++;
    }

    public void removeRef() {
      m_refCount--;
      if (m_refCount == 0) {
        this.close();
      }
    }
  }

  private static final Map<Integer, DataStore> m_handleMap = new HashMap<>();
  private static final Object m_handleLock = new Object();

  private static DataStore getForModule(int module) {
    synchronized (m_handleLock) {
      DataStore pcm = m_handleMap.get(module);
      if (pcm == null) {
        pcm = new DataStore(module);
      }
      pcm.addRef();
      return pcm;
    }
  }

  private static void freeModule(DataStore store) {
    synchronized (m_handleLock) {
      store.removeRef();
    }
  }

  /** Converts volts to PSI per the REV Analog Pressure Sensor datasheet. */
  private static double voltsToPsi(double sensorVoltage, double supplyVoltage) {
    return 250 * (sensorVoltage / supplyVoltage) - 25;
  }

  /** Converts PSI to volts per the REV Analog Pressure Sensor datasheet. */
  private static double psiToVolts(double pressure, double supplyVoltage) {
    return supplyVoltage * (0.004 * pressure + 0.1);
  }

  private final DataStore m_dataStore;
  private final int m_handle;

  /** Constructs a PneumaticHub with the default ID (1). */
  public PneumaticHub() {
    this(SensorUtil.getDefaultREVPHModule());
  }

  /**
   * Constructs a PneumaticHub.
   *
   * @param module module number to construct
   */
  public PneumaticHub(int module) {
    m_dataStore = getForModule(module);
    m_handle = m_dataStore.m_handle;
  }

  @Override
  public void close() {
    freeModule(m_dataStore);
  }

  @Override
  public boolean getCompressor() {
    return REVPHJNI.getCompressor(m_handle);
  }

  @Override
  public CompressorConfigType getCompressorConfigType() {
    return CompressorConfigType.fromValue(REVPHJNI.getCompressorConfig(m_handle));
  }

  @Override
  public boolean getPressureSwitch() {
    return REVPHJNI.getPressureSwitch(m_handle);
  }

  @Override
  public double getCompressorCurrent() {
    return REVPHJNI.getCompressorCurrent(m_handle);
  }

  @Override
  public void setSolenoids(int mask, int values) {
    REVPHJNI.setSolenoids(m_handle, mask, values);
  }

  @Override
  public int getSolenoids() {
    return REVPHJNI.getSolenoids(m_handle);
  }

  @Override
  public int getModuleNumber() {
    return m_dataStore.m_module;
  }

  @Override
  public void fireOneShot(int index) {
    REVPHJNI.fireOneShot(m_handle, index, m_dataStore.m_oneShotDurMs[index]);
  }

  @Override
  public void setOneShotDuration(int index, int durMs) {
    m_dataStore.m_oneShotDurMs[index] = durMs;
  }

  @Override
  public boolean checkSolenoidChannel(int channel) {
    return REVPHJNI.checkSolenoidChannel(channel);
  }

  @Override
  public int checkAndReserveSolenoids(int mask) {
    synchronized (m_dataStore.m_reserveLock) {
      if ((m_dataStore.m_reservedMask & mask) != 0) {
        return m_dataStore.m_reservedMask & mask;
      }
      m_dataStore.m_reservedMask |= mask;
      return 0;
    }
  }

  @Override
  public void unreserveSolenoids(int mask) {
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_reservedMask &= ~mask;
    }
  }

  @Override
  public Solenoid makeSolenoid(int channel) {
    return new Solenoid(m_dataStore.m_module, PneumaticsModuleType.REVPH, channel);
  }

  @Override
  public DoubleSolenoid makeDoubleSolenoid(int forwardChannel, int reverseChannel) {
    return new DoubleSolenoid(
        m_dataStore.m_module, PneumaticsModuleType.REVPH, forwardChannel, reverseChannel);
  }

  @Override
  public Compressor makeCompressor() {
    return new Compressor(m_dataStore.m_module, PneumaticsModuleType.REVPH);
  }

  @Override
  public boolean reserveCompressor() {
    synchronized (m_dataStore.m_reserveLock) {
      if (m_dataStore.m_compressorReserved) {
        return false;
      }
      m_dataStore.m_compressorReserved = true;
      return true;
    }
  }

  @Override
  public void unreserveCompressor() {
    synchronized (m_dataStore.m_reserveLock) {
      m_dataStore.m_compressorReserved = false;
    }
  }

  @Override
  public int getSolenoidDisabledList() {
    return REVPHJNI.getSolenoidDisabledList(m_handle);
  }

  /**
   * Disables the compressor. The compressor will not turn on until {@link
   * #enableCompressorDigital()}, {@link #enableCompressorAnalog(double, double)}, or {@link
   * #enableCompressorHybrid(double, double)} are called.
   */
  @Override
  public void disableCompressor() {
    REVPHJNI.setClosedLoopControlDisabled(m_handle);
  }

  @Override
  public void enableCompressorDigital() {
    REVPHJNI.setClosedLoopControlDigital(m_handle);
  }

  /**
   * Enables the compressor in analog mode. This mode uses an analog pressure sensor connected to
   * analog channel 0 to cycle the compressor. The compressor will turn on when the pressure drops
   * below {@code minPressure} and will turn off when the pressure reaches {@code maxPressure}.
   *
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value. Range 0-120 PSI.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value. Range 0-120 PSI. Must be larger then minPressure.
   */
  @Override
  public void enableCompressorAnalog(double minPressure, double maxPressure) {
    if (minPressure >= maxPressure) {
      throw new IllegalArgumentException("maxPressure must be greater than minPressure");
    }
    if (minPressure < 0 || minPressure > 120) {
      throw new IllegalArgumentException(
          "minPressure must be between 0 and 120 PSI, got " + minPressure);
    }
    if (maxPressure < 0 || maxPressure > 120) {
      throw new IllegalArgumentException(
          "maxPressure must be between 0 and 120 PSI, got " + maxPressure);
    }

    // Send the voltage as it would be if the 5V rail was at exactly 5V.
    // The firmware will compensate for the real 5V rail voltage, which
    // can fluctuate somewhat over time.
    double minAnalogVoltage = psiToVolts(minPressure, 5);
    double maxAnalogVoltage = psiToVolts(maxPressure, 5);
    REVPHJNI.setClosedLoopControlAnalog(m_handle, minAnalogVoltage, maxAnalogVoltage);
  }

  /**
   * Enables the compressor in hybrid mode. This mode uses both a digital pressure switch and an
   * analog pressure sensor connected to analog channel 0 to cycle the compressor.
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
   * @param minPressure The minimum pressure in PSI. The compressor will turn on when the pressure
   *     drops below this value and the pressure switch indicates that the system is not full. Range
   *     0-120 PSI.
   * @param maxPressure The maximum pressure in PSI. The compressor will turn off when the pressure
   *     reaches this value or the pressure switch is disconnected or indicates that the system is
   *     full. Range 0-120 PSI. Must be larger then minPressure.
   */
  @Override
  public void enableCompressorHybrid(double minPressure, double maxPressure) {
    if (minPressure >= maxPressure) {
      throw new IllegalArgumentException("maxPressure must be greater than minPressure");
    }
    if (minPressure < 0 || minPressure > 120) {
      throw new IllegalArgumentException(
          "minPressure must be between 0 and 120 PSI, got " + minPressure);
    }
    if (maxPressure < 0 || maxPressure > 120) {
      throw new IllegalArgumentException(
          "maxPressure must be between 0 and 120 PSI, got " + maxPressure);
    }

    // Send the voltage as it would be if the 5V rail was at exactly 5V.
    // The firmware will compensate for the real 5V rail voltage, which
    // can fluctuate somewhat over time.
    double minAnalogVoltage = psiToVolts(minPressure, 5);
    double maxAnalogVoltage = psiToVolts(maxPressure, 5);
    REVPHJNI.setClosedLoopControlHybrid(m_handle, minAnalogVoltage, maxAnalogVoltage);
  }

  /**
   * Returns the raw voltage of the specified analog input channel.
   *
   * @param channel The analog input channel to read voltage from.
   * @return The voltage of the specified analog input channel.
   */
  @Override
  public double getAnalogVoltage(int channel) {
    return REVPHJNI.getAnalogVoltage(m_handle, channel);
  }

  /**
   * Returns the pressure read by an analog pressure sensor on the specified analog input channel.
   *
   * @param channel The analog input channel to read pressure from.
   * @return The pressure read by an analog pressure sensor on the specified analog input channel.
   */
  @Override
  public double getPressure(int channel) {
    double sensorVoltage = REVPHJNI.getAnalogVoltage(m_handle, channel);
    double supplyVoltage = REVPHJNI.get5VVoltage(m_handle);
    return voltsToPsi(sensorVoltage, supplyVoltage);
  }

  /** Clears the sticky faults. */
  public void clearStickyFaults() {
    REVPHJNI.clearStickyFaults(m_handle);
  }

  /**
   * Returns the hardware and firmware versions of this device.
   *
   * @return The hardware and firmware versions.
   */
  public REVPHVersion getVersion() {
    return REVPHJNI.getVersion(m_handle);
  }

  /**
   * Returns the faults currently active on this device.
   *
   * @return The faults.
   */
  public REVPHFaults getFaults() {
    return REVPHJNI.getFaults(m_handle);
  }

  /**
   * Returns the sticky faults currently active on this device.
   *
   * @return The sticky faults.
   */
  public REVPHStickyFaults getStickyFaults() {
    return REVPHJNI.getStickyFaults(m_handle);
  }

  /**
   * Returns the current input voltage for this device.
   *
   * @return The input voltage.
   */
  public double getInputVoltage() {
    return REVPHJNI.getInputVoltage(m_handle);
  }

  /**
   * Returns the current voltage of the regulated 5v supply.
   *
   * @return The current voltage of the 5v supply.
   */
  public double get5VRegulatedVoltage() {
    return REVPHJNI.get5VVoltage(m_handle);
  }

  /**
   * Returns the total current (in amps) drawn by all solenoids.
   *
   * @return Total current drawn by all solenoids in amps.
   */
  public double getSolenoidsTotalCurrent() {
    return REVPHJNI.getSolenoidCurrent(m_handle);
  }

  /**
   * Returns the current voltage of the solenoid power supply.
   *
   * @return The current voltage of the solenoid power supply.
   */
  public double getSolenoidsVoltage() {
    return REVPHJNI.getSolenoidVoltage(m_handle);
  }
}
