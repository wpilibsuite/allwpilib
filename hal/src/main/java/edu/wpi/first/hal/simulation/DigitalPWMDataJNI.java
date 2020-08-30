/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class DigitalPWMDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelInitializedCallback(int index, int uid);
  public static native boolean getInitialized(int index);
  public static native void setInitialized(int index, boolean initialized);

  public static native int registerDutyCycleCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelDutyCycleCallback(int index, int uid);
  public static native double getDutyCycle(int index);
  public static native void setDutyCycle(int index, double dutyCycle);

  public static native int registerPinCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelPinCallback(int index, int uid);
  public static native int getPin(int index);
  public static native void setPin(int index, int pin);

  public static native void resetData(int index);

  public static native int findForChannel(int channel);
}
