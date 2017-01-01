/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class EncoderJNI extends JNIWrapper {

  public static native int initializeEncoder(int digitalSourceHandleA, int analogTriggerTypeA,
                                             int digitalSourceHandleB, int analogTriggerTypeB,
                                             boolean reverseDirection, int encodingType);

  public static native void freeEncoder(int encoderHandle);

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

  public static native void setEncoderReverseDirection(int encoderHandle,
                                                       boolean reverseDirection);

  public static native void setEncoderSamplesToAverage(int encoderHandle,
                                                       int samplesToAverage);

  public static native int getEncoderSamplesToAverage(int encoderHandle);

  public static native void setEncoderIndexSource(int encoderHandle, int digitalSourceHandle,
                                                  int analogTriggerType, int indexingType);

  @SuppressWarnings("AbbreviationAsWordInName")
  public static native int getEncoderFPGAIndex(int encoderHandle);

  public static native int getEncoderEncodingScale(int encoderHandle);

  public static native double getEncoderDecodingScaleFactor(int encoderHandle);

  public static native double getEncoderDistancePerPulse(int encoderHandle);

  public static native int getEncoderEncodingType(int encoderHandle);
}
