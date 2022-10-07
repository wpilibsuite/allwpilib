// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public final class HALUtil extends JNIWrapper {
  public static final int NULL_PARAMETER = -1005;
  public static final int SAMPLE_RATE_TOO_HIGH = 1001;
  public static final int VOLTAGE_OUT_OF_RANGE = 1002;
  public static final int LOOP_TIMING_ERROR = 1004;
  public static final int INCOMPATIBLE_STATE = 1015;
  public static final int ANALOG_TRIGGER_PULSE_OUTPUT_ERROR = -1011;
  public static final int NO_AVAILABLE_RESOURCES = -104;
  public static final int PARAMETER_OUT_OF_RANGE = -1028;

  public static final int RUNTIME_ROBORIO = 0;
  public static final int RUNTIME_ROBORIO2 = 1;
  public static final int RUNTIME_SIMULATION = 2;

  public static native short getFPGAVersion();

  public static native int getFPGARevision();

  public static native long getFPGATime();

  public static native int getHALRuntimeType();

  public static native boolean getFPGAButton();

  public static native String getHALErrorMessage(int code);

  public static native int getHALErrno();

  public static native String getHALstrerror(int errno);

  public static String getHALstrerror() {
    return getHALstrerror(getHALErrno());
  }

  private HALUtil() {}
}
