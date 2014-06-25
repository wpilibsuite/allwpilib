/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.hal.PDPJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;

/**
 * Class for getting voltage, current, and temperature from the CAN PDP
 * @author Thomas Clark
 */
public class PowerDistributionPanel extends SensorBase {
	public PowerDistributionPanel() {
	}

	/**
	 * @return The voltage of the PDP
	 */
	public double getVoltage() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double voltage = PDPJNI.getPDPVoltage(status.asIntBuffer());

		return voltage;
	}

	/**
	 * @return The temperature of the PDP in degrees Celsius
	 */
	public double getTemperature() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double temperature = PDPJNI.getPDPTemperature(status.asIntBuffer());

		return temperature;
	}

	/**
	 * @return The current of one of the PDP channels (channels 1-16) in Amperes
	 */
	public double getCurrent(int channel) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double current = PDPJNI.getPDPChannelCurrent((byte)channel, status.asIntBuffer());

		checkPDPChannel(channel);

		return current;
	}
}
