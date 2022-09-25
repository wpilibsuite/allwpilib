// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class EncoderJNI extends JNIWrapper {
  public static native int initializeEncoder(
      int digitalSourceHandleA,
      int analogTriggerTypeA,
      int digitalSourceHandleB,
      int analogTriggerTypeB,
      boolean reverseDirection,
      int encodingType);

  public static native void freeEncoder(int encoderHandle);

  public static native void setEncoderSimDevice(int handle, int device);

  public static native int getEncoder(int encoderHandle);

  public static native int getEncoderRaw(int encoderHandle);

  public static native int getEncodingScaleFactor(int encoderHandle);

  public static native void resetEncoder(int encoderHandle);

  public static native double getEncoderPeriod(int encoderHandle);

  public static native void setEncoderMaxPeriod(int encoderHandle, double maxPeriod);

  public static native boolean getEncoderStopped(int encoderHandle);

  public static native boolean getEncoderDirection(int encoderHandle);

  public static native double getEncoderDistance(int encoderHandle);

  public static native double getEncoderRate(int encoderHandle);

  public static native void setEncoderMinRate(int encoderHandle, double minRate);

  public static native void setEncoderDistancePerPulse(int encoderHandle, double distancePerPulse);

  public static native void setEncoderReverseDirection(int encoderHandle, boolean reverseDirection);

  public static native void setEncoderSamplesToAverage(int encoderHandle, int samplesToAverage);

  public static native int getEncoderSamplesToAverage(int encoderHandle);

  public static native void setEncoderIndexSource(
      int encoderHandle, int digitalSourceHandle, int analogTriggerType, int indexingType);

  public static native int getEncoderFPGAIndex(int encoderHandle);

  public static native int getEncoderEncodingScale(int encoderHandle);

  public static native double getEncoderDecodingScaleFactor(int encoderHandle);

  public static native double getEncoderDistancePerPulse(int encoderHandle);

  public static native int getEncoderEncodingType(int encoderHandle);
}
