package edu.wpi.first.wpilibj.hal;

public class PowerJNI extends JNIWrapper {
  public static native float getVinVoltage();

  public static native float getVinCurrent();

  public static native float getUserVoltage6V();

  public static native float getUserCurrent6V();

  public static native boolean getUserActive6V();

  public static native int getUserCurrentFaults6V();

  public static native float getUserVoltage5V();

  public static native float getUserCurrent5V();

  public static native boolean getUserActive5V();

  public static native int getUserCurrentFaults5V();

  public static native float getUserVoltage3V3();

  public static native float getUserCurrent3V3();

  public static native boolean getUserActive3V3();

  public static native int getUserCurrentFaults3V3();
}
