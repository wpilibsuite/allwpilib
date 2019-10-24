package edu.wpi.first.hal;

public class DutyCycleJNI extends JNIWrapper {
  public static native int initialize(int digitalSourceHandle, int analogTriggerType);
  public static native void free(int handle);
  
  public static native int getFrequency(int handle);
  public static native long getOutputRaw(int handle);
  public static native double getOutputScaled(int handle);
}