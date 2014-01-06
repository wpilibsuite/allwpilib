package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class RelayJNI extends DIOJNI {
	public static native void setRelayForward(ByteBuffer digital_port_pointer, byte on, IntBuffer status);
	public static native void setRelayReverse(ByteBuffer digital_port_pointer, byte on, IntBuffer status);
	public static native byte getRelayForward(ByteBuffer digital_port_pointer, IntBuffer status);
	public static native byte getRelayReverse(ByteBuffer digital_port_pointer, IntBuffer status);
}
