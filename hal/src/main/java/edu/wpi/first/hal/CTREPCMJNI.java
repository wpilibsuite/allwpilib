// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * CTRE Pneumatic Control Module (PCM) Functions.
 *
 * @see "CTREPCM.h"
 */
public class CTREPCMJNI extends JNIWrapper {
  /**
   * Initializes a PCM.
   *
   * @param module the CAN ID to initialize
   * @return the created PH handle
   * @see "HAL_InitializeCTREPCM"
   */
  public static native int initialize(int module);

  /**
   * Frees a PCM handle.
   *
   * @param handle the PCMhandle
   * @see "HAL_FreeCTREPCM"
   */
  public static native void free(int handle);

  /**
   * Checks if a solenoid channel number is valid.
   *
   * @param channel the channel to check
   * @return true if the channel is valid, otherwise false
   */
  public static native boolean checkSolenoidChannel(int channel);

  /**
   * Get whether compressor is turned on.
   *
   * @param handle the PCM handle
   * @return true if the compressor is turned on
   * @see "HAL_GetCTREPCMCompressor"
   */
  public static native boolean getCompressor(int handle);

  /**
   * Enables the compressor closed loop control using the digital pressure switch. The compressor
   * will turn on when the pressure switch indicates that the system is not full, and will turn off
   * when the pressure switch indicates that the system is full.
   *
   * @param handle the PCM handle
   * @param enabled true to enable closed loop control
   * @see "HAL_SetCTREPCMClosedLoopControl"
   */
  public static native void setClosedLoopControl(int handle, boolean enabled);

  /**
   * Get whether the PCM closed loop control is enabled.
   *
   * @param handle the PCM handle
   * @return True if closed loop control is enabled, otherwise false.
   */
  public static native boolean getClosedLoopControl(int handle);

  /**
   * Returns the state of the pressure switch.
   *
   * @param handle the PCM handle
   * @return True if pressure switch indicates that the system is full, otherwise false.
   * @see "HAL_GetCTREPCMPressureSwitch"
   */
  public static native boolean getPressureSwitch(int handle);

  /**
   * Returns the current drawn by the compressor.
   *
   * @param handle the PCM handle
   * @return The current drawn by the compressor in amps.
   * @see "HAL_GetCTREPCMCompressorCurrent"
   */
  public static native double getCompressorCurrent(int handle);

  /**
   * Return whether the compressor current is currently too high.
   *
   * @param handle the PCM handle
   * @return True if the compressor current is too high, otherwise false.
   * @see getCompressorCurrentTooHighStickyFault
   * @see "HAL_GetCTREPCMCompressorCurrentTooHighFault"
   */
  public static native boolean getCompressorCurrentTooHighFault(int handle);

  /**
   * Returns whether the compressor current has been too high since sticky faults were last cleared.
   * This fault is persistent and can be cleared by clearAllStickyFaults()
   *
   * @param handle the PCM handle
   * @return True if the compressor current has been too high since sticky faults were last cleared.
   * @see getCompressorCurrentTooHighFault
   * @see "HAL_GetCTREPCMCompressorCurrentTooHighStickyFault("
   */
  public static native boolean getCompressorCurrentTooHighStickyFault(int handle);

  /**
   * Returns whether the compressor is currently shorted.
   *
   * @param handle the PCM handle
   * @return True if the compressor is currently shorted, otherwise false.
   * @see getCompressorCurrentTooHighStickyFault
   * @see "HAL_GetCTREPCMCompressorShortedStickyFault"
   */
  public static native boolean getCompressorShortedFault(int handle);

  /**
   * Returns whether the compressor has been shorted since sticky faults were last cleared. This
   * fault is persistent and can be cleared by clearAllStickyFaults()
   *
   * @param handle the PCM handle
   * @return True if the compressor has been shorted since sticky faults were last cleared,
   *     otherwise false.
   * @see getCompressorShortedFault
   * @see "HAL_GetCTREPCMCompressorShortedFault"
   */
  public static native boolean getCompressorShortedStickyFault(int handle);

  /**
   * Returns whether the compressor is currently disconnected.
   *
   * @param handle the PCM handle
   * @return True if compressor is currently disconnected, otherwise false.
   * @see getCompressorShortedStickyFault
   * @see "HAL_GetCTREPCMCompressorNotConnectedFault"
   */
  public static native boolean getCompressorNotConnectedFault(int handle);

  /**
   * Returns whether the compressor has been disconnected since sticky faults were last cleared.
   * This fault is persistent and can be cleared by clearAllStickyFaults()
   *
   * @param handle the PCM handle
   * @return True if the compressor has been disconnected since sticky faults were last cleared,
   *     otherwise false.
   * @see getCompressorNotConnectedFault
   * @see "HAL_GetCTREPCMCompressorNotConnectedStickyFault"
   */
  public static native boolean getCompressorNotConnectedStickyFault(int handle);

  /**
   * Gets a bitmask of solenoid values.
   *
   * @param handle the PCM handle
   * @return Bitmask containing the state of the solenoids. The LSB represents solenoid 0.
   * @see "HAL_GetCTREPCMSolenoids"
   */
  public static native int getSolenoids(int handle);

  /**
   * Sets solenoids on a pneumatics module.
   *
   * @param handle the PCM handle
   * @param mask Bitmask indicating which solenoids to set. The LSB represents solenoid 0.
   * @param values Bitmask indicating the desired states of the solenoids. The LSB represents
   *     solenoid 0.
   * @see "HAL_SetCTREPCMSolenoids"
   */
  public static native void setSolenoids(int handle, int mask, int values);

  /**
   * Get a bitmask of disabled solenoids.
   *
   * @param handle the PCM handle
   * @return Bitmask indicating disabled solenoids. The LSB represents solenoid 0.
   * @see "HAL_GetCTREPCMSolenoidDisabledList"
   */
  public static native int getSolenoidDisabledList(int handle);

  /**
   * Returns whether the solenoid is currently reporting a voltage fault.
   *
   * @param handle the PCM handle
   * @return True if solenoid is reporting a fault, otherwise false.
   * @see getSolenoidVoltageStickyFault
   * @see "HAL_GetCTREPCMSolenoidVoltageFault"
   */
  public static native boolean getSolenoidVoltageFault(int handle);

  /**
   * Returns whether the solenoid has reported a voltage fault since sticky faults were last
   * cleared. This fault is persistent and can be cleared by clearAllStickyFaults()
   *
   * @param handle the PCM handle
   * @return True if solenoid is reporting a fault, otherwise false.
   * @see getSolenoidVoltageFault
   * @see "HAL_GetCTREPCMSolenoidVoltageStickyFault"
   */
  public static native boolean getSolenoidVoltageStickyFault(int handle);

  /**
   * Clears all sticky faults on this device.
   *
   * @param handle the PCM handle
   * @see "HAL_ClearAllCTREPCMStickyFaults"
   */
  public static native void clearAllStickyFaults(int handle);

  /**
   * Fire a single solenoid shot.
   *
   * @param handle the PCM handle
   * @param index solenoid index
   * @see "HAL_FireCTREPCMOneShot"
   */
  public static native void fireOneShot(int handle, int index);

  /**
   * Set the duration for a single solenoid shot.
   *
   * @param handle the PCM handle
   * @param index solenoid index
   * @param durMs shot duration in ms
   * @see "HAL_SetCTREPCMOneShotDuration"
   */
  public static native void setOneShotDuration(int handle, int index, int durMs);

  /** Utility class. */
  private CTREPCMJNI() {}
}
