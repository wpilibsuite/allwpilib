// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class ThreadsJNI extends JNIWrapper {
  public static native int getCurrentThreadPriority();

  public static native boolean getCurrentThreadIsRealTime();

  public static native boolean setCurrentThreadPriority(boolean realTime, int priority);
}
