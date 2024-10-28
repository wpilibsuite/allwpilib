// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * REV Pneumatic Hub (PH) HAL JNI functions.
 *
 * @see "REVPH.h"
 */
public class REVPHJNI extends JNIWrapper {
  /** Disabled. */
  public static final int COMPRESSOR_CONFIG_TYPE_DISABLED = 0;

  /** Digital. */
  public static final int COMPRESSOR_CONFIG_TYPE_DIGITAL = 1;

  /** Analog. */
  public static final int COMPRESSOR_CONFIG_TYPE_ANALOG = 2;

  /** Hybrid. */
  public static final int COMPRESSOR_CONFIG_TYPE_HYBRID = 3;

  /**
   * Initializes a PH.
   *
   * @param module the CAN ID to initialize
   * @return the created PH handle
   * @see "HAL_InitializeREVPH"
   */
  public static native int initialize(int module);

  /**
   * Frees a PH handle.
   *
   * @param handle the PH handle
   * @see "HAL_FreeREVPH"
   */
  public static native void free(int handle);

  /**
   * Checks if a solenoid channel number is valid.
   *
   * @param channel the channel to check
   * @return true if the channel is valid, otherwise false
   * @see "HAL_CheckREVPHSolenoidChannel"
   */
  public static native boolean checkSolenoidChannel(int channel);

  /**
   * Get whether compressor is turned on.
   *
   * @param handle the PH handle
   * @return true if the compressor is turned on
   * @see "HAL_GetREVPHCompressor"
   */
  public static native boolean getCompressor(int handle);

  /**
   * Send compressor configuration to the PH.
   *
   * @param handle the PH handle
   * @param minAnalogVoltage The compressor will turn on when the analog pressure sensor voltage
   *     drops below this value
   * @param maxAnalogVoltage The compressor will turn off when the analog pressure sensor reaches
   *     this value.
   * @param forceDisable Disable Compressor
   * @param useDigital use the digital pressure switch
   * @see "HAL_SetREVPHCompressorConfig"
   */
  public static native void setCompressorConfig(
      int handle,
      double minAnalogVoltage,
      double maxAnalogVoltage,
      boolean forceDisable,
      boolean useDigital);

  /**
   * Disable Compressor.
   *
   * @param handle the PH handle
   * @see "HAL_SetREVPHClosedLoopControlDisabled"
   */
  public static native void setClosedLoopControlDisabled(int handle);

  /**
   * Enables the compressor in digital mode using the digital pressure switch. The compressor will
   * turn on when the pressure switch indicates that the system is not full, and will turn off when
   * the pressure switch indicates that the system is full.
   *
   * @param handle the PH handle
   * @see "HAL_SetREVPHClosedLoopControlDigital"
   */
  public static native void setClosedLoopControlDigital(int handle);

  /**
   * Enables the compressor in analog mode. This mode uses an analog pressure sensor connected to
   * analog channel 0 to cycle the compressor. The compressor will turn on when the pressure drops
   * below minAnalogVoltage and will turn off when the pressure reaches maxAnalogVoltage. This mode
   * is only supported by the REV PH with the REV Analog Pressure Sensor connected to analog channel
   * 0.
   *
   * @param handle the PH handle
   * @param minAnalogVoltage The compressor will turn on when the analog pressure sensor voltage
   *     drops below this value
   * @param maxAnalogVoltage The compressor will turn off when the analog pressure sensor reaches
   *     this value.
   * @see "HAL_SetREVPHClosedLoopControlAnalog"
   */
  public static native void setClosedLoopControlAnalog(
      int handle, double minAnalogVoltage, double maxAnalogVoltage);

  /**
   * Enables the compressor in hybrid mode. This mode uses both a digital pressure switch and an
   * analog pressure sensor connected to analog channel 0 to cycle the compressor.
   *
   * <p>The compressor will turn on when \a both:
   *
   * <p>- The digital pressure switch indicates the system is not full AND - The analog pressure
   * sensor indicates that the pressure in the system is below the specified minimum pressure.
   *
   * <p>The compressor will turn off when \a either:
   *
   * <p>- The digital pressure switch is disconnected or indicates that the system is full OR - The
   * pressure detected by the analog sensor is greater than the specified maximum pressure.
   *
   * @param handle the PH handle
   * @param minAnalogVoltage The compressor will turn on when the analog pressure sensor voltage
   *     drops below this value and the pressure switch indicates that the system is not full.
   * @param maxAnalogVoltage The compressor will turn off when the analog pressure sensor reaches
   *     this value or the pressure switch is disconnected or indicates that the system is full.
   * @see "HAL_SetREVPHClosedLoopControlHybrid"
   */
  public static native void setClosedLoopControlHybrid(
      int handle, double minAnalogVoltage, double maxAnalogVoltage);

  /**
   * Get compressor configuration from the PH.
   *
   * @param handle the PH handle
   * @return compressor configuration
   * @see "HAL_GetREVPHCompressorConfig"
   */
  public static native int getCompressorConfig(int handle);

  /**
   * Returns the state of the digital pressure switch.
   *
   * @param handle the PH handle
   * @return True if pressure switch indicates that the system is full, otherwise false.
   * @see "HAL_GetREVPHPressureSwitch"
   */
  public static native boolean getPressureSwitch(int handle);

  /**
   * Returns the raw voltage of the specified analog input channel.
   *
   * @param handle the PH handle
   * @param channel The analog input channel to read voltage from.
   * @return The voltage of the specified analog input channel in volts.
   * @see "HAL_GetREVPHAnalogVoltage"
   */
  public static native double getAnalogVoltage(int handle, int channel);

  /**
   * Returns the current drawn by the compressor.
   *
   * @param handle the PH handle
   * @return The current drawn by the compressor in amps.
   * @see "HAL_GetREVPHCompressorCurrent"
   */
  public static native double getCompressorCurrent(int handle);

  /**
   * Gets a bitmask of solenoid values.
   *
   * @param handle the PH handle
   * @return Bitmask containing the state of the solenoids. The LSB represents solenoid 0.
   * @see "HAL_GetREVPHSolenoids"
   */
  public static native int getSolenoids(int handle);

  /**
   * Sets solenoids on a PH.
   *
   * @param handle the PH handle
   * @param mask Bitmask indicating which solenoids to set. The LSB represents solenoid 0.
   * @param values Bitmask indicating the desired states of the solenoids. The LSB represents
   *     solenoid 0.
   * @see "HAL_SetREVPHSolenoids"
   */
  public static native void setSolenoids(int handle, int mask, int values);

  /**
   * Fire a single solenoid shot for the specified duration.
   *
   * @param handle the PH handle
   * @param index solenoid index
   * @param durMs shot duration in ms
   * @see "HAL_FireREVPHOneShot"
   */
  public static native void fireOneShot(int handle, int index, int durMs);

  /**
   * Clears the sticky faults.
   *
   * @param handle the PH handle
   * @see "HAL_ClearREVPHStickyFaults"
   */
  public static native void clearStickyFaults(int handle);

  /**
   * Returns the current input voltage for the PH.
   *
   * @param handle the PH handle
   * @return The input voltage in volts.
   * @see "HAL_GetREVPHVoltage"
   */
  public static native double getInputVoltage(int handle);

  /**
   * Returns the current voltage of the regulated 5v supply.
   *
   * @param handle the PH handle
   * @return The current voltage of the 5v supply in volts.
   * @see "HAL_GetREVPH5VVoltage"
   */
  public static native double get5VVoltage(int handle);

  /**
   * Returns the total current drawn by all solenoids.
   *
   * @param handle the PH handle
   * @return Total current drawn by all solenoids in amps.
   * @see "HAL_GetREVPHSolenoidCurrent"
   */
  public static native double getSolenoidCurrent(int handle);

  /**
   * Returns the current voltage of the solenoid power supply.
   *
   * @param handle the PH handle
   * @return The current voltage of the solenoid power supply in volts.
   * @see "HAL_GetREVPHSolenoidVoltage"
   */
  public static native double getSolenoidVoltage(int handle);

  /**
   * Returns the sticky faults currently active on this device.
   *
   * @param handle the PH handle
   * @return The sticky faults.
   * @see "HAL_GetREVPHStickyFaults"
   */
  public static native int getStickyFaultsNative(int handle);

  /**
   * Returns the sticky faults currently active on this device.
   *
   * @param handle the PH handle
   * @return The sticky faults.
   * @see "HAL_GetREVPHStickyFaults"
   */
  public static REVPHStickyFaults getStickyFaults(int handle) {
    return new REVPHStickyFaults(getStickyFaultsNative(handle));
  }

  /**
   * Returns the faults currently active on the PH.
   *
   * @param handle the PH handle
   * @return The faults.
   * @see "HAL_GetREVPHFaults"
   */
  public static native int getFaultsNative(int handle);

  /**
   * Returns the faults currently active on the PH.
   *
   * @param handle the PH handle
   * @return The faults.
   * @see "HAL_GetREVPHFaults"
   */
  public static REVPHFaults getFaults(int handle) {
    return new REVPHFaults(getFaultsNative(handle));
  }

  /**
   * Get a bitmask of disabled solenoids.
   *
   * @param handle the PH handle
   * @return Bitmask indicating disabled solenoids. The LSB represents solenoid 0.
   * @see "HAL_GetREVPHSolenoidDisabledList"
   */
  public static native int getSolenoidDisabledList(int handle);

  /**
   * Returns the hardware and firmware versions of the PH.
   *
   * @param handle the PH handle
   * @return The hardware and firmware versions.
   * @see "HAL_GetREVPHVersion"
   */
  public static native REVPHVersion getVersion(int handle);

  /** Utility class. */
  private REVPHJNI() {}
}
