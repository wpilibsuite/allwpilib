// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** FIRST A301 motor controller HAL JNI functions. */
public class A301JNI extends JNIWrapper {
  public static final int MIN_DEVICE_ID = 0;
  public static final int MAX_DEVICE_ID = 62;
  public static final int DEFAULT_DEVICE_ID = 3;

  public static final int STATUS_0 = 0;
  public static final int STATUS_1 = 1;
  public static final int STATUS_2 = 2;
  public static final int STATUS_3 = 3;

  public static final int GEARBOX_RPM_UNKNOWN = 0;
  public static final int GEARBOX_RPM_215 = 1;
  public static final int GEARBOX_RPM_500 = 2;

  public static final int IDLE_MODE_COAST = 0;
  public static final int IDLE_MODE_BRAKE = 1;

  public static final int CONTROL_TYPE_DUTY_CYCLE = 0;
  public static final int CONTROL_TYPE_VELOCITY = 1;
  public static final int CONTROL_TYPE_VOLTAGE = 2;
  public static final int CONTROL_TYPE_RELATIVE_POSITION = 3;
  public static final int CONTROL_TYPE_ABSOLUTE_POSITION = 4;
  public static final int CONTROL_TYPE_CURRENT = 5;

  /**
   * Detects the device ID of an A301 attached to a CAN bus.
   *
   * @param busId CAN bus ID
   * @return detected device ID
   */
  public static native int detectDeviceId(int busId);

  /**
   * Initializes an A301.
   *
   * @param busId CAN bus ID
   * @param deviceId requested device ID
   * @return native A301 handle
   */
  public static native int initialize(int busId, int deviceId);

  /**
   * Frees an A301 handle.
   *
   * @param handle native A301 handle
   */
  public static native void free(int handle);

  /**
   * Returns whether an A301 device ID is valid.
   *
   * @param deviceId device ID to validate
   * @return true when the device ID is valid
   */
  public static native boolean checkDeviceId(int deviceId);

  /**
   * Returns the CAN bus ID associated with an A301 handle.
   *
   * @param handle native A301 handle
   * @return CAN bus ID
   */
  public static native int getBusId(int handle);

  /**
   * Returns the resolved device ID associated with an A301 handle.
   *
   * @param handle native A301 handle
   * @return resolved device ID
   */
  public static native int getDeviceId(int handle);

  /**
   * Returns the controller firmware version.
   *
   * @param handle native A301 handle
   * @return firmware and hardware version data
   */
  public static native A301FirmwareVersion getFirmwareVersion(int handle);

  /**
   * Reads and decodes periodic status frame 0.
   *
   * @param handle native A301 handle
   * @return decoded status frame
   */
  public static native A301PeriodicStatus0 getPeriodicStatus0(int handle);

  /**
   * Reads and decodes periodic status frame 1.
   *
   * @param handle native A301 handle
   * @return decoded status frame
   */
  public static native A301PeriodicStatus1 getPeriodicStatus1(int handle);

  /**
   * Reads and decodes periodic status frame 2.
   *
   * @param handle native A301 handle
   * @return decoded status frame
   */
  public static native A301PeriodicStatus2 getPeriodicStatus2(int handle);

  /**
   * Reads and decodes periodic status frame 3.
   *
   * @param handle native A301 handle
   * @return decoded status frame
   */
  public static native A301PeriodicStatus3 getPeriodicStatus3(int handle);

  /**
   * Sets the relative encoder position.
   *
   * @param handle native A301 handle
   * @param position position in motor rotations
   * @return HAL status code
   */
  public static native int setRelativeEncoderPosition(int handle, double position);

  /**
   * Sets the absolute encoder position.
   *
   * @param handle native A301 handle
   * @param position position in rotations
   * @return HAL status code
   */
  public static native int setAbsoluteEncoderPosition(int handle, double position);

  /**
   * Sends a repeating control setpoint.
   *
   * @param handle native A301 handle
   * @param value requested control value
   * @param controlType controller control mode
   * @param positionSpeed approach speed for position controls, or zero for maximum speed
   * @return HAL status code
   */
  public static native int setSetpoint(
      int handle, double value, int controlType, double positionSpeed);

  /**
   * Sets the controller idle mode.
   *
   * @param handle native A301 handle
   * @param idleMode idle mode
   * @return HAL status code
   */
  public static native int setIdleMode(int handle, int idleMode);

  /**
   * Returns the configured idle mode.
   *
   * @param handle native A301 handle
   * @return idle mode
   */
  public static native int getIdleMode(int handle);

  /**
   * Enables or disables continuous input for absolute position control.
   *
   * @param handle native A301 handle
   * @param enabled true to enable continuous input
   * @return HAL status code
   */
  public static native int setAbsolutePositionContinuousInput(int handle, boolean enabled);

  /**
   * Returns whether continuous input is enabled for absolute position control.
   *
   * @param handle native A301 handle
   * @return true when continuous input is enabled
   */
  public static native boolean getAbsolutePositionContinuousInput(int handle);

  /**
   * Sets the absolute encoder range offset.
   *
   * @param handle native A301 handle
   * @param offset offset in rotations, from -0.5 to 0.5
   * @return HAL status code
   */
  public static native int setAbsoluteEncoderRangeOffset(int handle, double offset);

  /**
   * Returns the absolute encoder range offset.
   *
   * @param handle native A301 handle
   * @return range offset in rotations
   */
  public static native double getAbsoluteEncoderRangeOffset(int handle);

  /**
   * Sets whether non-position control output is inverted.
   *
   * @param handle native A301 handle
   * @param inverted true to invert the output
   * @return HAL status code
   */
  public static native int setInverted(int handle, boolean inverted);

  /**
   * Returns whether non-position control output is inverted.
   *
   * @param handle native A301 handle
   * @return true when the output is inverted
   */
  public static native boolean getInverted(int handle);

  /**
   * Clears active and sticky faults and warnings.
   *
   * @param handle native A301 handle
   * @return HAL status code
   */
  public static native int clearFaults(int handle);

  /**
   * Sets a periodic status frame period.
   *
   * @param handle native A301 handle
   * @param frame periodic status frame
   * @param periodMs requested period in milliseconds, from 0 to 1000
   * @return HAL status code
   */
  public static native int setStatusFramePeriod(int handle, int frame, int periodMs);

  /**
   * Returns the effective periodic status frame period.
   *
   * @param handle native A301 handle
   * @param frame periodic status frame
   * @return period in milliseconds
   */
  public static native int getStatusFramePeriod(int handle, int frame);

  /** Prevents instantiation. */
  private A301JNI() {}
}
