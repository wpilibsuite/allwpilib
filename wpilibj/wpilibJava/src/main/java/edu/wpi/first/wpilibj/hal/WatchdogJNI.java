package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class WatchdogJNI extends JNIWrapper {
	public static native ByteBuffer initializeWatchdog(IntBuffer status);
	public static native void cleanWatchdog(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native byte feedWatchdog(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native void killWatchdog(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native double getWatchdogLastFed(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native double getWatchdogExpiration(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native void setWatchdogExpiration(ByteBuffer watchdog_pointer, double expiration, IntBuffer status);
	public static native byte getWatchdogEnabled(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native void setWatchdogEnabled(ByteBuffer watchdog_pointer, byte enabled, IntBuffer status);
	public static native byte isWatchdogAlive(ByteBuffer watchdog_pointer, IntBuffer status);
	public static native byte isWatchdogSystemActive(ByteBuffer watchdog_pointer, IntBuffer status);

}
