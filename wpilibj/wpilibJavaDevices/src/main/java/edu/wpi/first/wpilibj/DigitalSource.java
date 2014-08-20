/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * DigitalSource Interface. The DigitalSource represents all the possible inputs
 * for a counter or a quadrature encoder. The source may be either a digital
 * input or an analog input. If the caller just provides a channel, then a
 * digital input will be constructed and freed when finished for the source. The
 * source can either be a digital input or analog trigger but not both.
 */
public abstract class DigitalSource extends InterruptableSensorBase {

	protected static Resource channels = new Resource(kDigitalChannels);
	protected ByteBuffer m_port;
	protected int m_channel;

	protected void initDigitalPort(int channel, boolean input) {

		m_channel = channel;

		checkDigitalChannel(m_channel); // XXX: Replace with
										// HALLibrary.checkDigitalChannel when
										// implemented

		try {
			channels.allocate(m_channel);
		} catch (CheckedAllocationException ex) {
			throw new AllocationException("Digital input " + m_channel
					+ " is already allocated");
		}

		ByteBuffer port_pointer = DIOJNI.getPort((byte) channel);
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		m_port = DIOJNI.initializeDigitalPort(port_pointer, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		DIOJNI.allocateDIO(m_port, (byte) (input ? 1 : 0), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}

	@Override
	public void free() {
		channels.free(m_channel);
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		DIOJNI.freeDIO(m_port, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		m_channel = 0;
	}

	/**
	 * Get the channel routing number
	 *
	 * @return channel routing number
	 */
	@Override
	public int getChannelForRouting() {
		return m_channel;
	}

	/**
	 * Get the module routing number
	 *
	 * @return 0
	 */
	@Override
	public byte getModuleForRouting() {
		return 0;
	}

	/**
	 * Is this an analog trigger
	 * @return true if this is an analog trigger
	 */
	@Override
	public boolean getAnalogTriggerForRouting() {
		return false;
	}
}
