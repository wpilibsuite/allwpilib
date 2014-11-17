package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import edu.wpi.first.wpilibj.DriverStation;

public class HALUtil extends JNIWrapper {
	public static final int NULL_PARAMETER = -1005;
	public static final int SAMPLE_RATE_TOO_HIGH = 1001;
	public static final int VOLTAGE_OUT_OF_RANGE = 1002;
	public static final int LOOP_TIMING_ERROR = 1004;
	public static final int INCOMPATIBLE_STATE = 1015;
	public static final int ANALOG_TRIGGER_PULSE_OUTPUT_ERROR = -1011;
	public static final int NO_AVAILABLE_RESOURCES = -104;
	public static final int PARAMETER_OUT_OF_RANGE = -1028;

	//public static final int SEMAPHORE_WAIT_FOREVER = -1;
	//public static final int SEMAPHORE_Q_PRIORITY = 0x01;

	public static native ByteBuffer initializeMutexNormal();
	public static native void deleteMutex(ByteBuffer sem);
	public static native byte takeMutex(ByteBuffer sem);
	//public static native ByteBuffer initializeSemaphore(int initialValue);
	//public static native void deleteSemaphore(ByteBuffer sem);
	//public static native byte takeSemaphore(ByteBuffer sem, int timeout);
	public static native ByteBuffer initializeMultiWait();
	public static native void deleteMultiWait(ByteBuffer sem);
	public static native byte takeMultiWait(ByteBuffer sem, ByteBuffer m, int timeOut);
	public static native short getFPGAVersion(IntBuffer status);
	public static native int getFPGARevision(IntBuffer status);
	public static native long getFPGATime(IntBuffer status);
	public static native boolean getFPGAButton(IntBuffer status);

	public static native String getHALErrorMessage(int code);
	
	public static native int getHALErrno();
	public static native String getHALstrerror(int errno);
	public static String getHALstrerror(){
		return getHALstrerror(getHALErrno());
	}

	public static void checkStatus(IntBuffer status)
	{
		int s = status.get(0);
		if (s < 0)
		{
			String message = getHALErrorMessage(s);
			throw new RuntimeException(" Code: " + s + ". " + message);
		} else if (s > 0) {
			String message = getHALErrorMessage(s);
			DriverStation.reportError(message, true);
		}
	}

}
