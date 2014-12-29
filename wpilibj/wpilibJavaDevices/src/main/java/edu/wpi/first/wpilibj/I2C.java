/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;


import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.I2CJNI;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * I2C bus interface class.
 *
 * This class is intended to be used by sensor (and other I2C device) drivers.
 * It probably should not be used directly.
 *
 */
public class I2C extends SensorBase {
	public enum Port {kOnboard(0), kMXP(1);
		private int value;

		private Port(int value){
			this.value = value;
		}

		public int getValue(){
			return this.value;
		}
	};

	private Port m_port;
	private int m_deviceAddress;

    /**
     * Constructor.
     *
	 * @param port The I2C port the device is connected to.
     * @param deviceAddress
     *            The address of the device on the I2C bus.
     */
    public I2C(Port port, int deviceAddress) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);

        m_port = port;
        m_deviceAddress = deviceAddress;

		I2CJNI.i2CInitialize((byte)m_port.getValue(), status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());

        UsageReporting.report(tResourceType.kResourceType_I2C, deviceAddress);
    }

	/**
	 * Destructor.
	 */
	public void free() {
	}

	/**
	 * Generic transaction.
	 *
	 * This is a lower-level interface to the I2C hardware giving you more
	 * control over each transaction.
	 *
	 * @param dataToSend
	 *            Buffer of data to send as part of the transaction.
	 * @param sendSize
	 *            Number of bytes to send as part of the transaction. [0..6]
	 * @param dataReceived
	 *            Buffer to read data into.
	 * @param receiveSize
	 *            Number of bytes to read from the device. [0..7]
	 * @return Transfer Aborted... false for success, true for aborted.
	 */
	public synchronized boolean transaction(byte[] dataToSend, int sendSize,
			byte[] dataReceived, int receiveSize) {
		boolean aborted = true;

		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(sendSize);
		dataToSendBuffer.put(dataToSend);
		ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(receiveSize);

		aborted = I2CJNI
				.i2CTransaction((byte) m_port.getValue(), (byte) m_deviceAddress,
					dataToSendBuffer, (byte) sendSize,
					dataReceivedBuffer, (byte) receiveSize) != 0;
		/*if (status.get() == HALUtil.PARAMETER_OUT_OF_RANGE) {
			if (sendSize > 6) {
				throw new BoundaryException(BoundaryException.getMessage(
						sendSize, 0, 6));
			} else if (receiveSize > 7) {
				throw new BoundaryException(BoundaryException.getMessage(
						receiveSize, 0, 7));
			} else {
				throw new RuntimeException(
						HALLibrary.PARAMETER_OUT_OF_RANGE_MESSAGE);
			}
		}
		HALUtil.checkStatus(status);*/
		if(receiveSize > 0 && dataReceived != null)
		{
			dataReceivedBuffer.get(dataReceived);
		}
		return aborted;
	}

	/**
	 * Attempt to address a device on the I2C bus.
	 *
	 * This allows you to figure out if there is a device on the I2C bus that
	 * responds to the address specified in the constructor.
	 *
	 * @return Transfer Aborted... false for success, true for aborted.
	 */
	public boolean addressOnly() {
		return transaction(null, (byte) 0, null, (byte) 0);
	}

	/**
	 * Execute a write transaction with the device.
	 *
	 * Write a single byte to a register on a device and wait until the
	 * transaction is complete.
	 *
	 * @param registerAddress
	 *            The address of the register on the device to be written.
	 * @param data
	 *            The byte to write to the register on the device.
	 */
	public synchronized boolean write(int registerAddress, int data) {
		byte[] buffer = new byte[2];
		buffer[0] = (byte) registerAddress;
		buffer[1] = (byte) data;

		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(2);
		dataToSendBuffer.put(buffer);

		return I2CJNI.i2CWrite((byte)m_port.getValue(), (byte) m_deviceAddress, dataToSendBuffer, (byte)buffer.length) < 0;
	}

	/**
	 * Execute a write transaction with the device.
	 *
	 * Write multiple bytes to a register on a device and wait until the
	 * transaction is complete.
	 *
	 * @param data
	 *            The data to write to the device.
	 */
	public synchronized boolean writeBulk(byte[] data) {
		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(data.length);
		dataToSendBuffer.put(data);

		return I2CJNI.i2CWrite((byte)m_port.getValue(), (byte) m_deviceAddress, dataToSendBuffer, (byte)data.length) < 0;
	}

	/**
	 * Execute a read transaction with the device.
	 *
	 * Read 1 to 7 bytes from a device. Most I2C devices will auto-increment the
	 * register pointer internally allowing you to read up to 7 consecutive
	 * registers on a device in a single transaction.
	 *
	 * @param registerAddress
	 *            The register to read first in the transaction.
	 * @param count
	 *            The number of bytes to read in the transaction. [1..7]
	 * @param buffer
	 *            A pointer to the array of bytes to store the data read from
	 *            the device.
	 * @return Transfer Aborted... false for success, true for aborted.
	 */
	public boolean read(int registerAddress, int count, byte[] buffer) {
		BoundaryException.assertWithinBounds(count, 1, 7);
		if (buffer == null) {
			throw new NullPointerException("Null return buffer was given");
		}
		byte[] registerAddressArray = new byte[1];
		registerAddressArray[0] = (byte) registerAddress;

		return transaction(registerAddressArray, registerAddressArray.length,
				buffer, count);
	}

	/**
	 * Execute a read only transaction with the device.
	 *
	 * Read 1 to 7 bytes from a device. This method does not write any data to prompt
	 * the device.
	 *
	 * @param buffer
	 *            A pointer to the array of bytes to store the data read from
	 *            the device.
	 * @param count
	 *            The number of bytes to read in the transaction. [1..7]
	 * @return Transfer Aborted... false for success, true for aborted.
	 */
	public boolean readOnly(byte[] buffer, int count) {
		BoundaryException.assertWithinBounds(count, 1, 7);
		if (buffer == null) {
			throw new NullPointerException("Null return buffer was given");
		}

		ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(count);

		int retVal = I2CJNI.i2CRead((byte)m_port.getValue(), (byte) m_deviceAddress, dataReceivedBuffer, (byte)count);
		dataReceivedBuffer.get(buffer);
		return retVal < 0;
	}

	/**
	 * Send a broadcast write to all devices on the I2C bus.
	 *
	 * This is not currently implemented!
	 *
	 * @param registerAddress
	 *            The register to write on all devices on the bus.
	 * @param data
	 *            The value to write to the devices.
	 */
	public void broadcast(int registerAddress, int data) {
	}

	/**
	 * Verify that a device's registers contain expected values.
	 *
	 * Most devices will have a set of registers that contain a known value that
	 * can be used to identify them. This allows an I2C device driver to easily
	 * verify that the device contains the expected value.
	 *
	 * @pre The device must support and be configured to use register
	 *      auto-increment.
	 *
	 * @param registerAddress
	 *            The base register to start reading from the device.
	 * @param count
	 *            The size of the field to be verified.
	 * @param expected
	 *            A buffer containing the values expected from the device.
	 * @return true if the sensor was verified to be connected
	 */
	public boolean verifySensor(int registerAddress, int count, byte[] expected) {
		// TODO: Make use of all 7 read bytes
		byte[] deviceData = new byte[4];
		for (int i = 0, curRegisterAddress = registerAddress; i < count; i += 4, curRegisterAddress += 4) {
			int toRead = count - i < 4 ? count - i : 4;
			// Read the chunk of data. Return false if the sensor does not
			// respond.
			if (read(curRegisterAddress, toRead, deviceData)) {
				return false;
			}

			for (byte j = 0; j < toRead; j++) {
				if (deviceData[j] != expected[i + j]) {
					return false;
				}
			}
		}
		return true;
	}
}
