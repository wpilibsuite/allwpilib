package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.nio.LongBuffer;

public class AnalogJNI extends JNIWrapper {
	/**
	 * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:58</i><br>
	 * enum values
	 */
	public static interface AnalogTriggerType {
		/** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:54</i> */
		public static final int kInWindow = 0;
		/** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:55</i> */
		public static final int kState = 1;
		/** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:56</i> */
		public static final int kRisingPulse = 2;
		/** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:57</i> */
		public static final int kFallingPulse = 3;
	};

	public static native ByteBuffer initializeAnalogInputPort(ByteBuffer port_pointer, IntBuffer status);
	public static native ByteBuffer initializeAnalogOutputPort(ByteBuffer port_pointer, IntBuffer status);
	public static native byte checkAnalogModule(byte module);
	public static native byte checkAnalogInputChannel(int pin);
	public static native byte checkAnalogOutputChannel(int pin);
	public static native void setAnalogOutput(ByteBuffer port_pointer, double voltage, IntBuffer status);
	public static native double getAnalogOutput(ByteBuffer port_pointer, IntBuffer status);
	public static native void setAnalogSampleRate(double samplesPerSecond, IntBuffer status);
	public static native double getAnalogSampleRate(IntBuffer status);
	public static native void setAnalogAverageBits(ByteBuffer analog_port_pointer, int bits, IntBuffer status);
	public static native int getAnalogAverageBits(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native void setAnalogOversampleBits(ByteBuffer analog_port_pointer, int bits, IntBuffer status);
	public static native int getAnalogOversampleBits(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native short getAnalogValue(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native int getAnalogAverageValue(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native int getAnalogVoltsToValue(ByteBuffer analog_port_pointer, double voltage, IntBuffer status);
	public static native double getAnalogVoltage(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native double getAnalogAverageVoltage(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native int getAnalogLSBWeight(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native int getAnalogOffset(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native byte isAccumulatorChannel(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native void initAccumulator(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native void resetAccumulator(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native void setAccumulatorCenter(ByteBuffer analog_port_pointer, int center, IntBuffer status);
	public static native void setAccumulatorDeadband(ByteBuffer analog_port_pointer, int deadband, IntBuffer status);
	public static native long getAccumulatorValue(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native int getAccumulatorCount(ByteBuffer analog_port_pointer, IntBuffer status);
	public static native void getAccumulatorOutput(ByteBuffer analog_port_pointer, LongBuffer value, IntBuffer count, IntBuffer status);
	public static native ByteBuffer initializeAnalogTrigger(ByteBuffer port_pointer, IntBuffer index, IntBuffer status);
	public static native void cleanAnalogTrigger(ByteBuffer analog_trigger_pointer, IntBuffer status);
	public static native void setAnalogTriggerLimitsRaw(ByteBuffer analog_trigger_pointer, int lower, int upper, IntBuffer status);
	public static native void setAnalogTriggerLimitsVoltage(ByteBuffer analog_trigger_pointer, double lower, double upper, IntBuffer status);
	public static native void setAnalogTriggerAveraged(ByteBuffer analog_trigger_pointer, byte useAveragedValue, IntBuffer status);
	public static native void setAnalogTriggerFiltered(ByteBuffer analog_trigger_pointer, byte useFilteredValue, IntBuffer status);
	public static native byte getAnalogTriggerInWindow(ByteBuffer analog_trigger_pointer, IntBuffer status);
	public static native byte getAnalogTriggerTriggerState(ByteBuffer analog_trigger_pointer, IntBuffer status);
	public static native byte getAnalogTriggerOutput(ByteBuffer analog_trigger_pointer, int type, IntBuffer status);
}
