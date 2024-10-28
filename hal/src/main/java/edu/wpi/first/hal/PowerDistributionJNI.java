// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Power Distribution JNI Functions.
 *
 * @see "hal/PowerDistribution.h"
 */
public class PowerDistributionJNI extends JNIWrapper {
  /** Automatically determines the module type. */
  public static final int AUTOMATIC_TYPE = 0;

  /** CTRE (Cross The Road Electronics) Power Distribution Panel (PDP). */
  public static final int CTRE_TYPE = 1;

  /** REV Power Distribution Hub (PDH). */
  public static final int REV_TYPE = 2;

  /** Use the default module number for the selected module type. */
  public static final int DEFAULT_MODULE = -1;

  /**
   * Initializes a Power Distribution Panel.
   *
   * @param module the module number to initialize
   * @param type the type of module to initialize
   * @return the created PowerDistribution handle
   * @see "HAL_InitializePowerDistribution"
   */
  public static native int initialize(int module, int type);

  /**
   * Cleans a PowerDistribution module.
   *
   * @param handle the module handle
   * @see "HAL_CleanPowerDistribution"
   */
  public static native void free(int handle);

  /**
   * Gets the module number for a specific handle.
   *
   * @param handle the module handle
   * @return the module number
   * @see "HAL_GetPowerDistributionModuleNumber"
   */
  public static native int getModuleNumber(int handle);

  /**
   * Checks if a PowerDistribution module is valid.
   *
   * @param module the module to check
   * @param type the type of module
   * @return true if the module is valid, otherwise false
   * @see "HAL_CheckPowerDistributionModule"
   */
  public static native boolean checkModule(int module, int type);

  /**
   * Checks if a PowerDistribution channel is valid.
   *
   * @param handle the module handle
   * @param channel the channel to check
   * @return true if the channel is valid, otherwise false
   * @see "HAL_CheckPowerDistributionChannel"
   */
  public static native boolean checkChannel(int handle, int channel);

  /**
   * Gets the type of PowerDistribution module.
   *
   * @param handle the module handle
   * @return the type of module
   * @see "HAL_GetPowerDistributionType"
   */
  public static native int getType(int handle);

  /**
   * Gets the number of channels for this handle.
   *
   * @param handle the handle
   * @return number of channels
   * @see "HAL_GetPowerDistributionNumChannels"
   */
  public static native int getNumChannels(int handle);

  /**
   * Gets the temperature of the PowerDistribution.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @param handle the module handle
   * @return the module temperature (celsius)
   * @see "HAL_GetPowerDistributionTemperature"
   */
  public static native double getTemperature(int handle);

  /**
   * Gets the PowerDistribution input voltage.
   *
   * @param handle the module handle
   * @return the input voltage (volts)
   * @see "HAL_GetPowerDistributionVoltage"
   */
  public static native double getVoltage(int handle);

  /**
   * Gets the current of a specific PowerDistribution channel.
   *
   * @param handle the module handle
   * @param channel the channel
   * @return the channel current (amps)
   * @see "HAL_GetPowerDistributionChannelCurrent"
   */
  public static native double getChannelCurrent(int handle, int channel);

  /**
   * Gets the current of all channels on the PowerDistribution.
   *
   * <p>The array must be large enough to hold all channels.
   *
   * @param handle the module handle
   * @param currents the currents
   * @see "HAL_GetPowerDistributionAllChannelCurrents"
   */
  public static native void getAllCurrents(int handle, double[] currents);

  /**
   * Gets the total current of the PowerDistribution.
   *
   * @param handle the module handle
   * @return the total current (amps)
   * @see "HAL_GetPowerDistributionTotalCurrent"
   */
  public static native double getTotalCurrent(int handle);

  /**
   * Gets the total power of the Power Distribution Panel.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @param handle the module handle
   * @return the total power (watts)
   * @see "HAL_GetPowerDistributionTotalPower"
   */
  public static native double getTotalPower(int handle);

  /**
   * Gets the total energy of the Power Distribution Panel.
   *
   * <p>Not supported on the Rev PDH and does nothing.
   *
   * @param handle the module handle
   * @return the total energy (joules)
   * @see "HAL_GetPowerDistributionTotalEnergy"
   */
  public static native double getTotalEnergy(int handle);

  /**
   * Resets the Power Distribution Panel accumulated energy.
   *
   * <p>Not supported on the Rev PDH and returns 0.
   *
   * @param handle the module handle
   * @see "HAL_ClearPowerDistributionStickyFaults"
   */
  public static native void resetTotalEnergy(int handle);

  /**
   * Clears any PowerDistribution sticky faults.
   *
   * @param handle the module handle
   * @see "HAL_ClearPowerDistributionStickyFaults"
   */
  public static native void clearStickyFaults(int handle);

  /**
   * Returns true if switchable channel is powered on.
   *
   * <p>This is a REV PDH-specific function. This function will no-op on CTRE PDP.
   *
   * @param handle the module handle
   * @return the state of the switchable channel
   * @see "HAL_GetPowerDistributionSwitchableChannel"
   */
  public static native boolean getSwitchableChannel(int handle);

  /**
   * Power on/off switchable channel.
   *
   * <p>This is a REV PDH-specific function. This function will no-op on CTRE PDP.
   *
   * @param handle the module handle
   * @param enabled true to turn on switchable channel
   * @see "HAL_SetPowerDistributionSwitchableChannel"
   */
  public static native void setSwitchableChannel(int handle, boolean enabled);

  /**
   * Gets the PowerDistribution input voltage without throwing any errors.
   *
   * @param handle the module handle
   * @return the input voltage (volts)
   * @see "HAL_GetPowerDistributionVoltage"
   */
  public static native double getVoltageNoError(int handle);

  /**
   * Gets the current of a specific PowerDistribution channel without throwing any errors.
   *
   * @param handle the module handle
   * @param channel the channel
   * @return the channel current (amps)
   * @see "HAL_GetPowerDistributionChannelCurrent"
   */
  public static native double getChannelCurrentNoError(int handle, int channel);

  /**
   * Gets the total current of the PowerDistribution without throwing any errors.
   *
   * @param handle the module handle
   * @return the total current (amps)
   * @see "HAL_GetPowerDistributionTotalCurrent"
   */
  public static native double getTotalCurrentNoError(int handle);

  /**
   * Returns true if switchable channel is powered on without throwing any errors.
   *
   * <p>This is a REV PDH-specific function. This function will no-op on CTRE PDP.
   *
   * @param handle the module handle
   * @return the state of the switchable channel
   * @see "HAL_GetPowerDistributionSwitchableChannel"
   */
  public static native boolean getSwitchableChannelNoError(int handle);

  /**
   * Power on/off switchable channel without throwing any errors.
   *
   * <p>This is a REV PDH-specific function. This function will no-op on CTRE PDP.
   *
   * @param handle the module handle
   * @param enabled true to turn on switchable channel
   * @see "HAL_SetPowerDistributionSwitchableChannel"
   */
  public static native void setSwitchableChannelNoError(int handle, boolean enabled);

  /**
   * Get the current faults of the PowerDistribution.
   *
   * @param handle the module handle
   * @return the current faults
   * @see "HAL_GetPowerDistributionFaults"
   */
  public static native int getFaultsNative(int handle);

  /**
   * Get the current faults of the PowerDistribution.
   *
   * @param handle the module handle
   * @return the current faults
   * @see "HAL_GetPowerDistributionFaults"
   */
  public static PowerDistributionFaults getFaults(int handle) {
    return new PowerDistributionFaults(getFaultsNative(handle));
  }

  /**
   * Gets the sticky faults of the PowerDistribution.
   *
   * @param handle the module handle
   * @return the sticky faults
   * @see "HAL_GetPowerDistributionStickyFaults"
   */
  public static native int getStickyFaultsNative(int handle);

  /**
   * Gets the sticky faults of the PowerDistribution.
   *
   * @param handle the module handle
   * @return the sticky faults
   * @see "HAL_GetPowerDistributionStickyFaults"
   */
  public static PowerDistributionStickyFaults getStickyFaults(int handle) {
    return new PowerDistributionStickyFaults(getStickyFaultsNative(handle));
  }

  /**
   * Get the version of the PowerDistribution.
   *
   * @param handle the module handle
   * @return version
   * @see "HAL_GetPowerDistributionVersion"
   */
  public static native PowerDistributionVersion getVersion(int handle);

  /** Utility class. */
  private PowerDistributionJNI() {}
}
