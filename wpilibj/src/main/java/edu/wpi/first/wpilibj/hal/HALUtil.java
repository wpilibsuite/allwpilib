package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class HALUtil {

	public static void checkStatus(IntBuffer status) {
		int s = status.get(0);
		String message = "Code: " + s + ". " + HALLibrary.getHALErrorMessage(s);
		if (s == 0)
			return;
		else if (s == HALLibrary.NULL_PARAMETER
				|| s == HALLibrary.SAMPLE_RATE_TOO_HIGH
				|| s == HALLibrary.VOLTAGE_OUT_OF_RANGE)
			throw new IllegalArgumentException(message);
		else if (s == HALLibrary.LOOP_TIMING_ERROR) // TODO: Make these a
													// WPILibRuntimeException
			throw new RuntimeException(message);
		else if (s == HALLibrary.INCOMPATIBLE_STATE
				|| s == HALLibrary.ANALOG_TRIGGER_PULSE_OUTPUT_ERROR)
			throw new IllegalStateException(message);
		else if (s == HALLibrary.NO_AVAILABLE_RESOURCES)
			throw new RuntimeException(message);
		else
			throw new RuntimeException("Unknown error. Code: " + s + ". "
					+ message);
	}

}
