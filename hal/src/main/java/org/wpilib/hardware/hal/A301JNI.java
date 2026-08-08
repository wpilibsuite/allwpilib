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

  public static native int detectDeviceId(int busId);

  public static native int initialize(int busId, int deviceId);

  public static native void free(int handle);

  public static native boolean checkDeviceId(int deviceId);

  public static native int getBusId(int handle);

  public static native int getDeviceId(int handle);

  public static native A301FirmwareVersion getFirmwareVersion(int handle);

  public static native A301PeriodicStatus0 getPeriodicStatus0(int handle);

  public static native A301PeriodicStatus1 getPeriodicStatus1(int handle);

  public static native A301PeriodicStatus2 getPeriodicStatus2(int handle);

  public static native A301PeriodicStatus3 getPeriodicStatus3(int handle);

  public static native void setRelativeEncoderPosition(int handle, double position);

  public static native void setAbsoluteEncoderPosition(int handle, double position);

  public static native void setSetpoint(
      int handle, double value, int controlType, double positionSpeed);

  public static native void setIdleMode(int handle, int idleMode);

  public static native int getIdleMode(int handle);

  public static native void setAbsolutePositionContinuousInput(int handle, boolean enabled);

  public static native boolean getAbsolutePositionContinuousInput(int handle);

  public static native void setAbsoluteEncoderRangeOffset(int handle, double offset);

  public static native double getAbsoluteEncoderRangeOffset(int handle);

  public static native void setInverted(int handle, boolean inverted);

  public static native boolean getInverted(int handle);

  public static native void clearFaults(int handle);

  public static native void setStatusFramePeriod(int handle, int frame, int periodMs);

  public static native int getStatusFramePeriod(int handle, int frame);

  private A301JNI() {}
}
