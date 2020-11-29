/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AddressableLEDDataJNI extends JNIWrapper {
  public static native String getDisplayName(int index);
  public static native void setDisplayName(int index, String displayName);

  public static native int registerInitializedCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelInitializedCallback(int index, int uid);
  public static native boolean getInitialized(int index);
  public static native void setInitialized(int index, boolean initialized);

  public static native int registerOutputPortCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelOutputPortCallback(int index, int uid);
  public static native int getOutputPort(int index);
  public static native void setOutputPort(int index, int outputPort);

  public static native int registerLengthCallback(int index, NotifyCallback callback, boolean initialNotify);
  public static native void cancelLengthCallback(int index, int uid);
  public static native int getLength(int index);
  public static native void setLength(int index, int length);

  public static native int registerRunningCallback(int index, NotifyCallback callback, boolean initialNotify);
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
