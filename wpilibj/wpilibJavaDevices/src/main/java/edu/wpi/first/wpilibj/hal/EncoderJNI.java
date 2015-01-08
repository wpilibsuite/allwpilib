package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class EncoderJNI extends JNIWrapper {
	public static native ByteBuffer initializeEncoder(byte port_a_module, int port_a_pin, byte port_a_analog_trigger, byte port_b_module, int port_b_pin, byte port_b_analog_trigger, byte reverseDirection, IntBuffer index, IntBuffer status);
	public static native void freeEncoder(ByteBuffer encoder_pointer, IntBuffer status);
	public static native void resetEncoder(ByteBuffer encoder_pointer, IntBuffer status);
	public static native int getEncoder(ByteBuffer encoder_pointer, IntBuffer status);
	public static native double getEncoderPeriod(ByteBuffer encoder_pointer, IntBuffer status);
	public static native void setEncoderMaxPeriod(ByteBuffer encoder_pointer, double maxPeriod, IntBuffer status);
	public static native byte getEncoderStopped(ByteBuffer encoder_pointer, IntBuffer status);
	public static native byte getEncoderDirection(ByteBuffer encoder_pointer, IntBuffer status);
	public static native void setEncoderReverseDirection(ByteBuffer encoder_pointer, byte reverseDirection, IntBuffer status);
	public static native void setEncoderSamplesToAverage(ByteBuffer encoder_pointer, int samplesToAverage, IntBuffer status);
	public static native int getEncoderSamplesToAverage(ByteBuffer encoder_pointer, IntBuffer status);
	public static native void setEncoderIndexSource(ByteBuffer digital_port, int pin, boolean analogTrigger, boolean activeHigh, boolean edgeSensitive, IntBuffer status);
}
