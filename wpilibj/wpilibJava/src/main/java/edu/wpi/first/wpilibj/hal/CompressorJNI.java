package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class CompressorJNI extends JNIWrapper {
	public static native ByteBuffer initializeCompressor(byte module);
	public static native boolean checkCompressorModule(byte module);
	
	public static native boolean getCompressor(ByteBuffer pcm_pointer, IntBuffer status);
	
	public static native void setClosedLoopControl(ByteBuffer pcm_pointer, boolean value, IntBuffer status);
	public static native boolean getClosedLoopControl(ByteBuffer pcm_pointer, IntBuffer status);
	
	public static native boolean getPressureSwitch(ByteBuffer pcm_pointer, IntBuffer status);
	public static native float getCompressorCurrent(ByteBuffer pcm_pointer, IntBuffer status);
}
