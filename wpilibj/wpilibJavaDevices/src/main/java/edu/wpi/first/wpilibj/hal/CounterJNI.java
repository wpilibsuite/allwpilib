package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class CounterJNI extends JNIWrapper {
	public static native ByteBuffer initializeCounter(int mode, IntBuffer index, IntBuffer status);
	public static native void freeCounter(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterAverageSize(ByteBuffer counter_pointer, int size, IntBuffer status);
	public static native void setCounterUpSource(ByteBuffer counter_pointer, int pin, byte analogTrigger, IntBuffer status);
	public static native void setCounterUpSourceEdge(ByteBuffer counter_pointer, byte risingEdge, byte fallingEdge, IntBuffer status);
	public static native void clearCounterUpSource(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterDownSource(ByteBuffer counter_pointer, int pin, byte analogTrigger, IntBuffer status);
	public static native void setCounterDownSourceEdge(ByteBuffer counter_pointer, byte risingEdge, byte fallingEdge, IntBuffer status);
	public static native void clearCounterDownSource(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterUpDownMode(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterExternalDirectionMode(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterSemiPeriodMode(ByteBuffer counter_pointer, byte highSemiPeriod, IntBuffer status);
	public static native void setCounterPulseLengthMode(ByteBuffer counter_pointer, double threshold, IntBuffer status);
	public static native int getCounterSamplesToAverage(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterSamplesToAverage(ByteBuffer counter_pointer, int samplesToAverage, IntBuffer status);
	public static native void resetCounter(ByteBuffer counter_pointer, IntBuffer status);
	public static native int getCounter(ByteBuffer counter_pointer, IntBuffer status);
	public static native double getCounterPeriod(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterMaxPeriod(ByteBuffer counter_pointer, double maxPeriod, IntBuffer status);
	public static native void setCounterUpdateWhenEmpty(ByteBuffer counter_pointer, byte enabled, IntBuffer status);
	public static native byte getCounterStopped(ByteBuffer counter_pointer, IntBuffer status);
	public static native byte getCounterDirection(ByteBuffer counter_pointer, IntBuffer status);
	public static native void setCounterReverseDirection(ByteBuffer counter_pointer, byte reverseDirection, IntBuffer status);
}
