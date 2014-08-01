package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class DIOJNI extends JNIWrapper {
	public static native ByteBuffer initializeDigitalPort(ByteBuffer port_pointer, IntBuffer status);
	public static native byte allocateDIO(ByteBuffer digital_port_pointer, byte input, IntBuffer status);
	public static native void freeDIO(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void setDIO(ByteBuffer digital_port_pointer, short value, IntBuffer status);
	public static native byte getDIO(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native byte getDIODirection(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native void pulse(ByteBuffer digital_port_pointer, double pulseLength, IntBuffer status);
	public static native byte isPulsing(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native byte isAnyPulsing(IntBuffer status);
	public static native short getLoopTiming(IntBuffer status);
}
