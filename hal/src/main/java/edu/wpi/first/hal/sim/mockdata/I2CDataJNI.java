/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim.mockdata;

import edu.wpi.first.hal.sim.BufferCallback;
import edu.wpi.first.hal.sim.ConstBufferCallback;
import edu.wpi.first.hal.sim.NotifyCallback;
import edu.wpi.first.hal.JNIWrapper;

public class I2CDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelInitializedCallback(int index, int uid);
  public static native boolean getInitialized(int index);
  public static native void setInitialized(int index, boolean initialized);

  public static native int registerReadCallback(int index, BufferCallback callback);
  public static native void cancelReadCallback(int index, int uid);

  public static native int registerWriteCallback(int index, ConstBufferCallback callback);
  public static native void cancelWriteCallback(int index, int uid);

  public static native void resetData(int index);
}
