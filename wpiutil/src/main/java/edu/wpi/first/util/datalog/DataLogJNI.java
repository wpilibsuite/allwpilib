// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.WPIUtilJNI;

public class DataLogJNI extends WPIUtilJNI {
  static native long create(String dir, String filename, double period, String extraHeader);

  static native void setFilename(long impl, String filename);

  static native void flush(long impl);

  static native void pause(long impl);

  static native void resume(long impl);

  static native int start(long impl, String name, String type, String metadata, long timestamp);

  static native void finish(long impl, int entry, long timestamp);

  static native void setMetadata(long impl, int entry, String metadata, long timestamp);

  static native void close(long impl);

  static native void appendRaw(long impl, int entry, byte[] data, long timestamp);

  static native void appendBoolean(long impl, int entry, boolean value, long timestamp);

  static native void appendInteger(long impl, int entry, long value, long timestamp);

  static native void appendFloat(long impl, int entry, float value, long timestamp);

  static native void appendDouble(long impl, int entry, double value, long timestamp);

  static native void appendString(long impl, int entry, String value, long timestamp);

  static native void appendBooleanArray(long impl, int entry, boolean[] value, long timestamp);

  static native void appendIntegerArray(long impl, int entry, long[] value, long timestamp);

  static native void appendFloatArray(long impl, int entry, float[] value, long timestamp);

  static native void appendDoubleArray(long impl, int entry, double[] value, long timestamp);

  static native void appendStringArray(long impl, int entry, String[] value, long timestamp);
}
