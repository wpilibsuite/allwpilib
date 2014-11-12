package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class PowerJNI extends JNIWrapper {
	public static native float getVinVoltage(IntBuffer status);
	public static native float getVinCurrent(IntBuffer status);
	public static native float getUserVoltage6V(IntBuffer status);
	public static native float getUserCurrent6V(IntBuffer status);
	public static native boolean getUserActive6V(IntBuffer status);
	public static native int getUserCurrentFaults6V(IntBuffer status);
	public static native float getUserVoltage5V(IntBuffer status);
	public static native float getUserCurrent5V(IntBuffer status);
	public static native boolean getUserActive5V(IntBuffer status);
	public static native int getUserCurrentFaults5V(IntBuffer status);
	public static native float getUserVoltage3V3(IntBuffer status);
	public static native float getUserCurrent3V3(IntBuffer status);
	public static native boolean getUserActive3V3(IntBuffer status);
	public static native int getUserCurrentFaults3V3(IntBuffer status);
}
