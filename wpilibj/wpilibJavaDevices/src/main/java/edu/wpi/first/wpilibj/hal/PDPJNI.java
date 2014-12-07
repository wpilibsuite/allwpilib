package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class PDPJNI extends JNIWrapper {
	public static native double getPDPTemperature(IntBuffer status);
	public static native double getPDPVoltage(IntBuffer status);
	public static native double getPDPChannelCurrent(byte channel, IntBuffer status);
	public static native double getPDPTotalCurrent(IntBuffer status);
	public static native double getPDPTotalPower(IntBuffer status);
	public static native double getPDPTotalEnergy(IntBuffer status);
	public static native void resetPDPTotalEnergy(IntBuffer status);
	public static native void clearPDPStickyFaults(IntBuffer status);
}