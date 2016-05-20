/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.LongBuffer;

public class AnalogJNI extends JNIWrapper {
  /**
   * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:58</i><br> enum values
   */
  public interface AnalogTriggerType {
    /**
     * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:54</i>
     */
    int kInWindow = 0;
    /**
     * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:55</i>
     */
    int kState = 1;
    /**
     * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:56</i>
     */
    int kRisingPulse = 2;
    /**
     * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:57</i>
     */
    int kFallingPulse = 3;
  }

  public static native long initializeAnalogInputPort(long portPointer);

  public static native void freeAnalogInputPort(long portPointer);

  public static native long initializeAnalogOutputPort(long portPointer);

  public static native void freeAnalogOutputPort(long portPointer);

  public static native boolean checkAnalogModule(byte module);

  public static native boolean checkAnalogInputChannel(int pin);

  public static native boolean checkAnalogOutputChannel(int pin);

  public static native void setAnalogOutput(long portPointer, double voltage);

  public static native double getAnalogOutput(long portPointer);

  public static native void setAnalogSampleRate(double samplesPerSecond);

  public static native double getAnalogSampleRate();

  public static native void setAnalogAverageBits(long analogPortPointer, int bits);

  public static native int getAnalogAverageBits(long analogPortPointer);

  public static native void setAnalogOversampleBits(long analogPortPointer, int bits);

  public static native int getAnalogOversampleBits(long analogPortPointer);

  public static native short getAnalogValue(long analogPortPointer);

  public static native int getAnalogAverageValue(long analogPortPointer);

  public static native int getAnalogVoltsToValue(long analogPortPointer, double voltage);

  public static native double getAnalogVoltage(long analogPortPointer);

  public static native double getAnalogAverageVoltage(long analogPortPointer);

  public static native int getAnalogLSBWeight(long analogPortPointer);

  public static native int getAnalogOffset(long analogPortPointer);

  public static native boolean isAccumulatorChannel(long analogPortPointer);

  public static native void initAccumulator(long analogPortPointer);

  public static native void resetAccumulator(long analogPortPointer);

  public static native void setAccumulatorCenter(long analogPortPointer, int center);

  public static native void setAccumulatorDeadband(long analogPortPointer, int deadband);

  public static native long getAccumulatorValue(long analogPortPointer);

  public static native int getAccumulatorCount(long analogPortPointer);

  public static native void getAccumulatorOutput(long analogPortPointer, LongBuffer value,
                                                 IntBuffer count);

  public static native long initializeAnalogTrigger(long portPointer, IntBuffer index);

  public static native void cleanAnalogTrigger(long analogTriggerPointer);

  public static native void setAnalogTriggerLimitsRaw(long analogTriggerPointer, int lower,
                                                      int upper);

  public static native void setAnalogTriggerLimitsVoltage(long analogTriggerPointer,
                                                          double lower, double upper);

  public static native void setAnalogTriggerAveraged(long analogTriggerPointer,
                                                     boolean useAveragedValue);

  public static native void setAnalogTriggerFiltered(long analogTriggerPointer,
                                                     boolean useFilteredValue);

  public static native boolean getAnalogTriggerInWindow(long analogTriggerPointer);

  public static native boolean getAnalogTriggerTriggerState(long analogTriggerPointer);

  public static native boolean getAnalogTriggerOutput(long analogTriggerPointer, int type);
}
