/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.datalog;

import edu.wpi.first.wpiutil.WPIUtilJNI;
import edu.wpi.first.wpiutil.datalog.DataLog.DataLogConfig;

public class DataLogJNI extends WPIUtilJNI {
  public static native String getDataType(long impl);
  public static native String getDataLayout(long impl);
  public static native int getRecordSize(long impl);
  public static native boolean isFixedSize(long impl);
  public static native void flush(long impl);

  public static native int getSize(long impl);

  public static native int find(long impl, long timestamp, int first);
  public static native int find(long impl, long timestamp, int first, int last);

  public static native boolean check(long impl, String dataType, int recordSize, boolean checkType, boolean checkLayout, boolean checkSize);

  public static native void close(long impl);

  public static native long open(String filename, String dataType, String dataLayout, int recordSize, int disp, DataLogConfig config);

  public static native long openRaw(String filename, DataLogConfig config);
  public static native boolean appendRaw(long impl, byte[] data);
  public static native boolean appendRawTime(long impl, long timestamp, byte[] data);
  public static native byte[] readRaw(long impl, int n);

  public static native long openBoolean(String filename, int disp, DataLogConfig config);
  public static native boolean appendBoolean(long impl, boolean value);
  public static native boolean appendBooleanTime(long impl, long timestamp, boolean value);

  public static native long openDouble(String filename, int disp, DataLogConfig config);
  public static native boolean appendDouble(long impl, double value);
  public static native boolean appendDoubleTime(long impl, long timestamp, double value);

  public static native long openString(String filename, int disp, DataLogConfig config);
  public static native boolean appendString(long impl, String value);
  public static native boolean appendStringTime(long impl, long timestamp, String value);

  public static native long openBooleanArray(String filename, int disp, DataLogConfig config);
  public static native boolean appendBooleanArray(long impl, boolean[] value);
  public static native boolean appendBooleanArrayTime(long impl, long timestamp, boolean[] value);

  public static native long openDoubleArray(String filename, int disp, DataLogConfig config);
  public static native boolean appendDoubleArray(long impl, double[] value);
  public static native boolean appendDoubleArrayTime(long impl, long timestamp, double[] value);

  public static native long openStringArray(String filename, int disp, DataLogConfig config);
  public static native boolean appendStringArray(long impl, String[] value);
  public static native boolean appendStringArrayTime(long impl, long timestamp, String[] value);
}
