/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
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
  public enum Port {
    kOnboard(0), kMXP(1);
    private int value;

    private Port(int value) {
      this.value = value;
    }

    public int getValue() {
      return this.value;
    }
  };

  private Port m_port;
  private int m_deviceAddress;

  /**
   * Constructor.
   *
   * @param port The I2C port the device is connected to.
   * @param deviceAddress The address of the device on the I2C bus.
   */
  public I2C(Port port, int deviceAddress) {
    m_port = port;
    m_deviceAddress = deviceAddress;

    I2CJNI.i2CInitialize((byte) m_port.getValue());

    UsageReporting.report(tResourceType.kResourceType_I2C, deviceAddress);
  }

  /**
   * Destructor.
   */
  public void free() {}

  /**
   * Generic transaction.
   *
   * This is a lower-level interface to the I2C hardware giving you more control
   * over each transaction.
   *
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize Number of bytes to read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean transaction(byte[] dataToSend, int sendSize, byte[] dataReceived,
      int receiveSize) {
    boolean aborted = true;

    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(sendSize);
    if (sendSize > 0 && dataToSend != null) {
      dataToSendBuffer.put(dataToSend);
    }
    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(receiveSize);

    aborted =
        I2CJNI.i2CTransaction((byte) m_port.getValue(), (byte) m_deviceAddress, dataToSendBuffer,
            (byte) sendSize, dataReceivedBuffer, (byte) receiveSize) != 0;
    if (receiveSize > 0 && dataReceived != null) {
      dataReceivedBuffer.get(dataReceived);
    }
    return aborted;
  }

  /**
   * Generic transaction.
   *
   * This is a lower-level interface to the I2C hardware giving you more control
   * over each transaction.
   *
   * @param dataToSend Buffer of data to send as part of the transaction. Must
   *        be allocated using ByteBuffer.allocateDirect().
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into. Must be allocated using
   *        ByteBuffer.allocateDirect().
   * @param receiveSize Number of bytes to read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean transaction(ByteBuffer dataToSend, int sendSize, ByteBuffer dataReceived,
      int receiveSize) {
    boolean aborted = true;

    if (!dataToSend.isDirect())
      throw new IllegalArgumentException("dataToSend must be a direct buffer");
    if (dataToSend.capacity() < sendSize)
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + sendSize);
    if (!dataReceived.isDirect())
      throw new IllegalArgumentException("dataReceived must be a direct buffer");
    if (dataReceived.capacity() < receiveSize)
      throw new IllegalArgumentException("dataReceived is too small, must be at least " + receiveSize);

    return I2CJNI.i2CTransaction((byte) m_port.getValue(), (byte) m_deviceAddress, dataToSend, (byte) sendSize, dataReceived, (byte) receiveSize) != 0;
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
    return transaction((byte[]) null, (byte) 0, (byte[]) null, (byte) 0);
  }

  /**
   * Execute a write transaction with the device.
   *
   * Write a single byte to a register on a device and wait until the
   * transaction is complete.
   *
   * @param registerAddress The address of the register on the device to be
   *        written.
   * @param data The byte to write to the register on the device.
   */
  public synchronized boolean write(int registerAddress, int data) {
    byte[] buffer = new byte[2];
    buffer[0] = (byte) registerAddress;
    buffer[1] = (byte) data;

    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(2);
    dataToSendBuffer.put(buffer);

    return I2CJNI.i2CWrite((byte) m_port.getValue(), (byte) m_deviceAddress, dataToSendBuffer,
        (byte) buffer.length) < 0;
  }

  /**
   * Execute a write transaction with the device.
   *
   * Write multiple bytes to a register on a device and wait until the
   * transaction is complete.
   *
   * @param data The data to write to the device.
   */
  public synchronized boolean writeBulk(byte[] data) {
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(data.length);
    dataToSendBuffer.put(data);

    return I2CJNI.i2CWrite((byte) m_port.getValue(), (byte) m_deviceAddress, dataToSendBuffer,
        (byte) data.length) < 0;
  }

  /**
   * Execute a write transaction with the device.
   *
   * Write multiple bytes to a register on a device and wait until the
   * transaction is complete.
   *
   * @param data The data to write to the device. Must be created using
   *        ByteBuffer.allocateDirect().
   */
  public synchronized boolean writeBulk(ByteBuffer data, int size) {
    if (!data.isDirect())
      throw new IllegalArgumentException("must be a direct buffer");
    if (data.capacity() < size)
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);

    return I2CJNI.i2CWrite((byte) m_port.getValue(), (byte) m_deviceAddress, data,
        (byte) size) < 0;
  }

  /**
   * Execute a read transaction with the device.
   *
   * Read bytes from a device. Most I2C devices will auto-increment the
   * register pointer internally allowing you to read consecutive
   * registers on a device in a single transaction.
   *
   * @param registerAddress The register to read first in the transaction.
   * @param count The number of bytes to read in the transaction.
   * @param buffer A pointer to the array of bytes to store the data read from
   *        the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean read(int registerAddress, int count, byte[] buffer) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count +
                                  " given");
    }

    if (buffer == null) {
      throw new NullPointerException("Null return buffer was given");
    }
    byte[] registerAddressArray = new byte[1];
    registerAddressArray[0] = (byte) registerAddress;

    return transaction(registerAddressArray, registerAddressArray.length, buffer, count);
  }

  /**
   * Execute a read transaction with the device.
   *
   * Read bytes from a device. Most I2C devices will auto-increment the
   * register pointer internally allowing you to read consecutive
   * registers on a device in a single transaction.
   *
   * @param registerAddress The register to read first in the transaction.
   * @param count The number of bytes to read in the transaction.
   * @param buffer A buffer to store the data read from the device. Must be
   *        created using ByteBuffer.allocateDirect().
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean read(int registerAddress, int count, ByteBuffer buffer) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count +
                                  " given");
    }

    if (!buffer.isDirect())
      throw new IllegalArgumentException("must be a direct buffer");
    if (buffer.capacity() < count)
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);

    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(1);
    dataToSendBuffer.put(0, (byte) registerAddress);

    return transaction(dataToSendBuffer, 1, buffer, count);
  }

  /**
   * Execute a read only transaction with the device.
   *
   * Read bytes from a device. This method does not write any data to prompt
   * the device.
   *
   * @param buffer A pointer to the array of bytes to store the data read from
   *        the device.
   * @param count The number of bytes to read in the transaction.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean readOnly(byte[] buffer, int count) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count +
                                  " given");
    }

    if (buffer == null) {
      throw new NullPointerException("Null return buffer was given");
    }

    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(count);

    int retVal =
        I2CJNI.i2CRead((byte) m_port.getValue(), (byte) m_deviceAddress, dataReceivedBuffer,
            (byte) count);
    dataReceivedBuffer.get(buffer);
    return retVal < 0;
  }

  /**
   * Execute a read only transaction with the device.
   *
   * Read bytes from a device. This method does not write any data to prompt
   * the device.
   *
   * @param buffer A pointer to the array of bytes to store the data read from
   *        the device. Must be created using ByteBuffer.allocateDirect().
   * @param count The number of bytes to read in the transaction.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean readOnly(ByteBuffer buffer, int count) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count +
                                  " given");
    }

    if (!buffer.isDirect())
      throw new IllegalArgumentException("must be a direct buffer");
    if (buffer.capacity() < count)
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);

    return I2CJNI.i2CRead((byte) m_port.getValue(), (byte) m_deviceAddress, buffer,
            (byte) count) < 0;
  }

  /**
   * Send a broadcast write to all devices on the I2C bus.
   *
   * This is not currently implemented!
   *
   * @param registerAddress The register to write on all devices on the bus.
   * @param data The value to write to the devices.
   */
  public void broadcast(int registerAddress, int data) {}

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
   * @param registerAddress The base register to start reading from the device.
   * @param count The size of the field to be verified.
   * @param expected A buffer containing the values expected from the device.
   * @return true if the sensor was verified to be connected
   */
  public boolean verifySensor(int registerAddress, int count, byte[] expected) {
    // TODO: Make use of all 7 read bytes
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(1);

    ByteBuffer deviceData = ByteBuffer.allocateDirect(4);
    for (int i = 0, curRegisterAddress = registerAddress; i < count; i += 4, curRegisterAddress +=
        4) {
      int toRead = count - i < 4 ? count - i : 4;
      // Read the chunk of data. Return false if the sensor does not
      // respond.
      dataToSendBuffer.put(0, (byte) curRegisterAddress);
      if (transaction(dataToSendBuffer, 1, deviceData, toRead)) {
        return false;
      }

      for (byte j = 0; j < toRead; j++) {
        if (deviceData.get(j) != expected[i + j]) {
          return false;
        }
      }
    }
    return true;
  }
}
