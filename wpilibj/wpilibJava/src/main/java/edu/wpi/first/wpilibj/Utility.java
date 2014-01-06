/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;

import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.parsing.IUtility;

/**
 * Contains global utility functions
 */
public class Utility implements IUtility {

	private Utility() {
	}

	/**
	 * Return the FPGA Version number. For now, expect this to be 2009.
	 * 
	 * @return FPGA Version number.
	 */
	int getFPGAVersion() {
		IntBuffer status = IntBuffer.allocate(1);
		int value = HALLibrary.getFPGAVersion(status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Return the FPGA Revision number. The format of the revision is 3 numbers.
	 * The 12 most significant bits are the Major Revision. the next 8 bits are
	 * the Minor Revision. The 12 least significant bits are the Build Number.
	 * 
	 * @return FPGA Revision number.
	 */
	long getFPGARevision() {
		IntBuffer status = IntBuffer.allocate(1);
		int value = HALLibrary.getFPGARevision(status);
		HALUtil.checkStatus(status);
		return (long) value;
	}

	/**
	 * Read the microsecond timer from the FPGA.
	 * 
	 * @return The current time in microseconds according to the FPGA.
	 */
	public static long getFPGATime() {
		IntBuffer status = IntBuffer.allocate(1);
		int value = HALLibrary.getFPGATime(status);
		HALUtil.checkStatus(status);
		return (long) value;
	}

	/**
	 * Control whether to send System.err output to the driver station's error
	 * pane.
	 * 
	 * @param enabled
	 *            if true, send error stream to driver station, otherwise
	 *            disable sending error stream
	 */
	public static void sendErrorStreamToDriverStation(boolean enabled) {
		final String url = "dserror:edu.wpi.first.wpilibj.Utility"; // the path
																	// is just a
																	// comment.
		/* XXX: Code Review!
		Isolate isolate = VM.getCurrentIsolate();
		if (enabled) {
			isolate.addErr(url);
		} else {
			isolate.removeErr(url);
		} */
	}

}
