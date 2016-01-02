/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class HALUtil extends JNIWrapper {
  public static final int NULL_PARAMETER = -1005;
  public static final int SAMPLE_RATE_TOO_HIGH = 1001;
  public static final int VOLTAGE_OUT_OF_RANGE = 1002;
  public static final int LOOP_TIMING_ERROR = 1004;
  public static final int INCOMPATIBLE_STATE = 1015;
  public static final int ANALOG_TRIGGER_PULSE_OUTPUT_ERROR = -1011;
  public static final int NO_AVAILABLE_RESOURCES = -104;
  public static final int PARAMETER_OUT_OF_RANGE = -1028;

  // public static final int SEMAPHORE_WAIT_FOREVER = -1;
  // public static final int SEMAPHORE_Q_PRIORITY = 0x01;

  public static native long initializeMutexNormal();

  public static native void deleteMutex(long sem);

  public static native void takeMutex(long sem);

  // public static native long initializeSemaphore(int initialValue);
  // public static native void deleteSemaphore(long sem);
  // public static native byte takeSemaphore(long sem, int timeout);
  public static native long initializeMultiWait();

  public static native void deleteMultiWait(long sem);

  public static native void takeMultiWait(long sem, long m);

  public static native short getFPGAVersion();

  public static native int getFPGARevision();

  public static native long getFPGATime();

  public static native boolean getFPGAButton();

  public static native String getHALErrorMessage(int code);

  public static native int getHALErrno();

  public static native String getHALstrerror(int errno);

  public static String getHALstrerror() {
    return getHALstrerror(getHALErrno());
  }
}
