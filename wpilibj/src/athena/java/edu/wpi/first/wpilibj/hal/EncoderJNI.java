/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class EncoderJNI extends JNIWrapper {
  public static native long initializeEncoder(byte port_a_module, int port_a_pin,
      boolean port_a_analog_trigger, byte port_b_module, int port_b_pin, boolean port_b_analog_trigger,
      boolean reverseDirection, IntBuffer index);

  public static native void freeEncoder(long encoder_pointer);

  public static native void resetEncoder(long encoder_pointer);

  public static native int getEncoder(long encoder_pointer);

  public static native double getEncoderPeriod(long encoder_pointer);

  public static native void setEncoderMaxPeriod(long encoder_pointer, double maxPeriod);

  public static native boolean getEncoderStopped(long encoder_pointer);

  public static native boolean getEncoderDirection(long encoder_pointer);

  public static native void setEncoderReverseDirection(long encoder_pointer,
      boolean reverseDirection);

  public static native void setEncoderSamplesToAverage(long encoder_pointer,
      int samplesToAverage);

  public static native int getEncoderSamplesToAverage(long encoder_pointer);

  public static native void setEncoderIndexSource(long digital_port, int pin,
      boolean analogTrigger, boolean activeHigh, boolean edgeSensitive);
}
