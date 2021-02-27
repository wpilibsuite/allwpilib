// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AddressableLEDDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerOutputPortCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelOutputPortCallback(int index, int uid);

  public static native int getOutputPort(int index);

  public static native void setOutputPort(int index, int outputPort);

  public static native int registerLengthCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelLengthCallback(int index, int uid);

  public static native int getLength(int index);

  public static native void setLength(int index, int length);

  public static native int registerRunningCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelRunningCallback(int index, int uid);

  public static native boolean getRunning(int index);

  public static native void setRunning(int index, boolean running);

  public static native int registerDataCallback(int index, ConstBufferCallback callback);

  public static native void cancelDataCallback(int index, int uid);

  public static native byte[] getData(int index);

  public static native void setData(int index, byte[] data);

  public static native void resetData(int index);

  public static native int findForChannel(int channel);
}
