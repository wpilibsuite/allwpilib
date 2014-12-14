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
	 * @return The current of one of the PDP channels (channels 0-15) in Amperes
	 */
	public double getCurrent(int channel) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double current = PDPJNI.getPDPChannelCurrent((byte)channel, status.asIntBuffer());

		checkPDPChannel(channel);

		return current;
	}

	/**
	*	@return The current of all the channels
	*/
	public double getTotalCurrent(){
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double current = PDPJNI.getPDPTotalCurrent(status.asIntBuffer());

		return current;
	}

	/**
	*	@return the total power
	*/
	public double getTotalPower(){
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double power = PDPJNI.getPDPTotalPower(status.asIntBuffer());

		return power;

	}

	public double getTotalEnergy(){
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		double energy = PDPJNI.getPDPTotalEnergy(status.asIntBuffer());

		return energy;
	}

	public void resetTotalEnergy(){
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

		PDPJNI.resetPDPTotalEnergy(status.asIntBuffer());
	}

	public void clearStickyFaults(){
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		PDPJNI.clearPDPStickyFaults(status.asIntBuffer());
	}

}
