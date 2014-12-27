package edu.wpi.first.wpilibj.hal;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class SolenoidJNI extends JNIWrapper {
	public static native ByteBuffer initializeSolenoidPort(ByteBuffer portPointer, IntBuffer status);
	public static native ByteBuffer getPortWithModule(byte module, byte channel);
	public static native void setSolenoid(ByteBuffer port, byte on, IntBuffer status);
	public static native byte getSolenoid(ByteBuffer port, IntBuffer status);

	public static native byte getPCMSolenoidBlackList(ByteBuffer pcm_pointer, IntBuffer status);
	public static native boolean getPCMSolenoidVoltageStickyFault(ByteBuffer pcm_pointer, IntBuffer status);
	public static native boolean getPCMSolenoidVoltageFault(ByteBuffer pcm_pointer, IntBuffer status);
	public static native void clearAllPCMStickyFaults(ByteBuffer pcm_pointer, IntBuffer status);
}
