/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class EncoderJNI extends JNIWrapper {
  public static native long initializeEncoder(byte portAModule, int portAPin,
                                              boolean portAAnalogTrigger, byte portBModule,
                                              int portBPin, boolean portBAnalogTrigger,
                                              boolean reverseDirection, IntBuffer index);

  public static native void freeEncoder(long encoderPointer);

  public static native void resetEncoder(long encoderPointer);

  public static native int getEncoder(long encoderPointer);

  public static native double getEncoderPeriod(long encoderPointer);

  public static native void setEncoderMaxPeriod(long encoderPointer, double maxPeriod);

  public static native boolean getEncoderStopped(long encoderPointer);

  public static native boolean getEncoderDirection(long encoderPointer);

  public static native void setEncoderReverseDirection(long encoderPointer,
                                                       boolean reverseDirection);

  public static native void setEncoderSamplesToAverage(long encoderPointer,
                                                       int samplesToAverage);

  public static native int getEncoderSamplesToAverage(long encoderPointer);

  public static native void setEncoderIndexSource(long digitalPort, int pin,
                                                  boolean analogTrigger, boolean activeHigh,
                                                  boolean edgeSensitive);
}
