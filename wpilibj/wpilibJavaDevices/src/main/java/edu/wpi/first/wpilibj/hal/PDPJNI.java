package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class PDPJNI extends JNIWrapper {
	public static native void initializePDP(int module);
	public static native double getPDPTemperature(IntBuffer status, int module);
	public static native double getPDPVoltage(IntBuffer status, int module);
	public static native double getPDPChannelCurrent(byte channel, IntBuffer status, int module);
	public static native double getPDPTotalCurrent(IntBuffer status, int module);
	public static native double getPDPTotalPower(IntBuffer status, int module);
	public static native double getPDPTotalEnergy(IntBuffer status, int module);
	public static native void resetPDPTotalEnergy(IntBuffer status, int module);
	public static native void clearPDPStickyFaults(IntBuffer status, int module);
}