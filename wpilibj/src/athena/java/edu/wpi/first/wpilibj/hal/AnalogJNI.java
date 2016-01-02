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
   * <i>native declaration :
   * AthenaJava\target\native\include\HAL\Analog.h:58</i><br>
   * enum values
   */
  public static interface AnalogTriggerType {
    /**
     * <i>native declaration :
     * AthenaJava\target\native\include\HAL\Analog.h:54</i>
     */
    public static final int kInWindow = 0;
    /**
     * <i>native declaration :
     * AthenaJava\target\native\include\HAL\Analog.h:55</i>
     */
    public static final int kState = 1;
    /**
     * <i>native declaration :
     * AthenaJava\target\native\include\HAL\Analog.h:56</i>
     */
    public static final int kRisingPulse = 2;
    /**
     * <i>native declaration :
     * AthenaJava\target\native\include\HAL\Analog.h:57</i>
     */
    public static final int kFallingPulse = 3;
  };

  public static native long initializeAnalogInputPort(long port_pointer);

  public static native void freeAnalogInputPort(long port_pointer);

  public static native long initializeAnalogOutputPort(long port_pointer);

  public static native void freeAnalogOutputPort(long port_pointer);

  public static native boolean checkAnalogModule(byte module);

  public static native boolean checkAnalogInputChannel(int pin);

  public static native boolean checkAnalogOutputChannel(int pin);

  public static native void setAnalogOutput(long port_pointer, double voltage);

  public static native double getAnalogOutput(long port_pointer);

  public static native void setAnalogSampleRate(double samplesPerSecond);

  public static native double getAnalogSampleRate();

  public static native void setAnalogAverageBits(long analog_port_pointer, int bits);

  public static native int getAnalogAverageBits(long analog_port_pointer);

  public static native void setAnalogOversampleBits(long analog_port_pointer, int bits);

  public static native int getAnalogOversampleBits(long analog_port_pointer);

  public static native short getAnalogValue(long analog_port_pointer);

  public static native int getAnalogAverageValue(long analog_port_pointer);

  public static native int getAnalogVoltsToValue(long analog_port_pointer, double voltage);

  public static native double getAnalogVoltage(long analog_port_pointer);

  public static native double getAnalogAverageVoltage(long analog_port_pointer);

  public static native int getAnalogLSBWeight(long analog_port_pointer);

  public static native int getAnalogOffset(long analog_port_pointer);

  public static native boolean isAccumulatorChannel(long analog_port_pointer);

  public static native void initAccumulator(long analog_port_pointer);

  public static native void resetAccumulator(long analog_port_pointer);

  public static native void setAccumulatorCenter(long analog_port_pointer, int center);

  public static native void setAccumulatorDeadband(long analog_port_pointer, int deadband);

  public static native long getAccumulatorValue(long analog_port_pointer);

  public static native int getAccumulatorCount(long analog_port_pointer);

  public static native void getAccumulatorOutput(long analog_port_pointer, LongBuffer value,
      IntBuffer count);

  public static native long initializeAnalogTrigger(long port_pointer, IntBuffer index);

  public static native void cleanAnalogTrigger(long analog_trigger_pointer);

  public static native void setAnalogTriggerLimitsRaw(long analog_trigger_pointer, int lower,
      int upper);

  public static native void setAnalogTriggerLimitsVoltage(long analog_trigger_pointer,
      double lower, double upper);

  public static native void setAnalogTriggerAveraged(long analog_trigger_pointer,
      boolean useAveragedValue);

  public static native void setAnalogTriggerFiltered(long analog_trigger_pointer,
      boolean useFilteredValue);

  public static native boolean getAnalogTriggerInWindow(long analog_trigger_pointer);

  public static native boolean getAnalogTriggerTriggerState(long analog_trigger_pointer);

  public static native boolean getAnalogTriggerOutput(long analog_trigger_pointer, int type);
}
