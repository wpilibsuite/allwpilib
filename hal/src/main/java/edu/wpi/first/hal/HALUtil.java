// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Hardware Abstraction Layer (HAL) Utilities JNI Functions.
 *
 * @see "hal/HALBase.h"
 */
public final class HALUtil extends JNIWrapper {
  /** A pointer parameter to a method is NULL. */
  public static final int NULL_PARAMETER = -1005;

  /** Analog module sample rate is too high. */
  public static final int SAMPLE_RATE_TOO_HIGH = 1001;

  /** Voltage to convert to raw value is out of range [0; 5]. */
  public static final int VOLTAGE_OUT_OF_RANGE = 1002;

  /** Digital module loop timing is not the expected value. */
  public static final int LOOP_TIMING_ERROR = 1004;

  /** The operation cannot be completed. */
  public static final int INCOMPATIBLE_STATE = 1015;

  /** Attempted to read AnalogTrigger pulse output. */
  public static final int ANALOG_TRIGGER_PULSE_OUTPUT_ERROR = -1011;

  /** No available resources to allocate. */
  public static final int NO_AVAILABLE_RESOURCES = -104;

  /** A parameter is out of range. */
  public static final int PARAMETER_OUT_OF_RANGE = -1028;

  /** roboRIO 1.0. */
  public static final int RUNTIME_ROBORIO = 0;

  /** roboRIO 2.0. */
  public static final int RUNTIME_ROBORIO2 = 1;

  /** Simulation runtime. */
  public static final int RUNTIME_SIMULATION = 2;

  /**
   * Returns the FPGA Version number.
   *
   * <p>For now, expect this to be competition year.
   *
   * @return FPGA Version number.
   * @see "HAL_GetFPGAVersion"
   */
  public static native short getFPGAVersion();

  /**
   * Returns the FPGA Revision number.
   *
   * <p>The format of the revision is 3 numbers. The 12 most significant bits are the Major
   * Revision. the next 8 bits are the Minor Revision. The 12 least significant bits are the Build
   * Number.
   *
   * @return FPGA Revision number.
   * @see "HAL_GetFPGARevision"
   */
  public static native int getFPGARevision();

  /**
   * Returns the roboRIO serial number.
   *
   * @return The roboRIO serial number.
   * @see "HAL_GetSerialNumber"
   */
  public static native String getSerialNumber();

  /**
   * Returns the comments from the roboRIO web interface.
   *
   * @return The comments string.
   * @see "HAL_GetComments"
   */
  public static native String getComments();

  /**
   * Returns the team number configured for the robot controller.
   *
   * @return team number, or 0 if not found.
   * @see "HAL_GetTeamNumber"
   */
  public static native int getTeamNumber();

  /**
   * Reads the microsecond-resolution timer on the FPGA.
   *
   * @return The current time in microseconds according to the FPGA (since FPGA reset).
   */
  public static native long getFPGATime();

  /**
   * Returns the runtime type of the HAL.
   *
   * @return HAL Runtime Type
   * @see RUNTIME_ROBORIO
   * @see RUNTIME_ROBORIO2
   * @see RUNTIME_SIMULATION
   * @see "HAL_GetRuntimeType"
   */
  public static native int getHALRuntimeType();

  /**
   * Gets the state of the "USER" button on the roboRIO.
   *
   * <p>Warning: the User Button is used to stop user programs from automatically loading if it is
   * held for more then 5 seconds. Because of this, it's not recommended to be used by teams for any
   * other purpose.
   *
   * @return true if the button is currently pressed down
   * @see "HAL_GetFPGAButton"
   */
  public static native boolean getFPGAButton();

  /**
   * Gets the error message for a specific status code.
   *
   * @param code the status code
   * @return the error message for the code. This does not need to be freed.
   * @see "HAL_GetErrorMessage"
   */
  public static native String getHALErrorMessage(int code);

  /**
   * Get the last HAL error code.
   *
   * @return error code
   */
  public static native int getHALErrno();

  /**
   * Returns the textual description of the system error code.
   *
   * @param errno errno to get description of
   * @return description of errno
   * @see "std:strerror"
   */
  public static native String getHALstrerror(int errno);

  /**
   * Gets the error message for the last HAL error.
   *
   * @return the error message for the code.
   */
  public static String getHALstrerror() {
    return getHALstrerror(getHALErrno());
  }

  private HALUtil() {}
}
