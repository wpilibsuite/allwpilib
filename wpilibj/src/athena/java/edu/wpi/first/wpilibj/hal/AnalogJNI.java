/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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

  public static native int initializeAnalogInputPort(int halPortHandle);

  public static native void freeAnalogInputPort(int portHandle);

  public static native int initializeAnalogOutputPort(int halPortHandle);

  public static native void freeAnalogOutputPort(int portHandle);

  public static native boolean checkAnalogModule(byte module);

  public static native boolean checkAnalogInputChannel(int channel);

  public static native boolean checkAnalogOutputChannel(int channel);

  public static native void setAnalogOutput(int portHandle, double voltage);

  public static native double getAnalogOutput(int portHandle);

  public static native void setAnalogSampleRate(double samplesPerSecond);

  public static native double getAnalogSampleRate();

  public static native void setAnalogAverageBits(int analogPortHandle, int bits);

  public static native int getAnalogAverageBits(int analogPortHandle);

  public static native void setAnalogOversampleBits(int analogPortHandle, int bits);

  public static native int getAnalogOversampleBits(int analogPortHandle);

  public static native short getAnalogValue(int analogPortHandle);

  public static native int getAnalogAverageValue(int analogPortHandle);

  public static native int getAnalogVoltsToValue(int analogPortHandle, double voltage);

  public static native double getAnalogVoltage(int analogPortHandle);

  public static native double getAnalogAverageVoltage(int analogPortHandle);

  public static native int getAnalogLSBWeight(int analogPortHandle);

  public static native int getAnalogOffset(int analogPortHandle);

  public static native boolean isAccumulatorChannel(int analogPortHandle);

  public static native void initAccumulator(int analogPortHandle);

  public static native void resetAccumulator(int analogPortHandle);

  public static native void setAccumulatorCenter(int analogPortHandle, int center);

  public static native void setAccumulatorDeadband(int analogPortHandle, int deadband);

  public static native long getAccumulatorValue(int analogPortHandle);

  public static native int getAccumulatorCount(int analogPortHandle);

  public static native void getAccumulatorOutput(int analogPortHandle, LongBuffer value,
                                                 LongBuffer count);

  public static native int initializeAnalogTrigger(int analogInputHandle, IntBuffer index);

  public static native void cleanAnalogTrigger(int analogTriggerHandle);

  public static native void setAnalogTriggerLimitsRaw(int analogTriggerHandle, int lower,
                                                      int upper);

  public static native void setAnalogTriggerLimitsVoltage(int analogTriggerHandle,
                                                          double lower, double upper);

  public static native void setAnalogTriggerAveraged(int analogTriggerHandle,
                                                     boolean useAveragedValue);

  public static native void setAnalogTriggerFiltered(int analogTriggerHandle,
                                                     boolean useFilteredValue);

  public static native boolean getAnalogTriggerInWindow(int analogTriggerHandle);

  public static native boolean getAnalogTriggerTriggerState(int analogTriggerHandle);

  public static native boolean getAnalogTriggerOutput(int analogTriggerHandle, int type);
}
