package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;

public class HALUtil extends JNIWrapper {
	public static final int NULL_PARAMETER = -5;
	public static final int SAMPLE_RATE_TOO_HIGH = 1;
	public static final int VOLTAGE_OUT_OF_RANGE = 2;
	public static final int LOOP_TIMING_ERROR = 4;
	public static final int INCOMPATIBLE_STATE = 15;
	public static final int ANALOG_TRIGGER_PULSE_OUTPUT_ERROR = -11;
	public static final int NO_AVAILABLE_RESOURCES = -4;
	public static final int PARAMETER_OUT_OF_RANGE = -28;

	//public static final int SEMAPHORE_WAIT_FOREVER = -1;
	//public static final int SEMAPHORE_Q_PRIORITY = 0x01;

	public static native ByteBuffer initializeMutexNormal();
	public static native void deleteMutex(ByteBuffer sem);
	public static native byte takeMutex(ByteBuffer sem);
	//public static native ByteBuffer initializeSemaphore(int initialValue);
	//public static native void deleteSemaphore(ByteBuffer sem);
	//public static native byte takeSemaphore(ByteBuffer sem, int timeout);
	public static native short getFPGAVersion(IntBuffer status);
	public static native int getFPGARevision(IntBuffer status);
	public static native long getFPGATime(IntBuffer status);
	public static native boolean getFPGAButton(IntBuffer status);

	public static native String getHALErrorMessage(int code);

	public static void checkStatus(IntBuffer status)
	{
		int s = status.get(0);
		if (s != 0)
		{
			String message = "Code: " + s + ". " + HALUtil.getHALErrorMessage(s);
			if (s == HALUtil.NULL_PARAMETER
				|| s == HALUtil.SAMPLE_RATE_TOO_HIGH
				|| s == HALUtil.VOLTAGE_OUT_OF_RANGE)
				throw new IllegalArgumentException(message);
			else if (s == HALUtil.LOOP_TIMING_ERROR)
				throw new RuntimeException(message);
			else if (s == HALUtil.INCOMPATIBLE_STATE
				|| s == HALUtil.ANALOG_TRIGGER_PULSE_OUTPUT_ERROR)
				throw new IllegalStateException(message);
			else if (s == HALUtil.NO_AVAILABLE_RESOURCES)
				throw new RuntimeException(message);
			else
				throw new RuntimeException("Unknown error. Code: " + s + ". " + message);
		}
	}

}
