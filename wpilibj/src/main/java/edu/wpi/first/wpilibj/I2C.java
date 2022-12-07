// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.I2CJNI;
import edu.wpi.first.hal.util.BoundaryException;
import java.nio.ByteBuffer;

/**
 * I2C bus interface class.
 *
 * <p>This class is intended to be used by sensor (and other I2C device) drivers. It probably should
 * not be used directly.
 *
 * <p>The Onboard I2C port is subject to system lockups. See <a
 * href="https://docs.wpilib.org/en/stable/docs/yearly-overview/known-issues.html#onboard-i2c-causing-system-lockups">
 * WPILib Known Issues</a> page for details.
 */
public class I2C implements AutoCloseable {
  public enum Port {
    kOnboard(0),
    kMXP(1);

    public final int value;

    Port(int value) {
      this.value = value;
    }
  }

  private final int m_port;
  private final int m_deviceAddress;

  /**
   * Constructor.
   *
   * @param port The I2C port the device is connected to.
   * @param deviceAddress The address of the device on the I2C bus.
   */
  public I2C(Port port, int deviceAddress) {
    m_port = port.value;
    m_deviceAddress = deviceAddress;

    if (port == I2C.Port.kOnboard) {
      DriverStation.reportWarning(
          "Onboard I2C port is subject to system lockups. See Known Issues page for details",
          false);
    }

    I2CJNI.i2CInitialize((byte) port.value);

    HAL.report(tResourceType.kResourceType_I2C, deviceAddress);
  }

  public int getPort() {
    return m_port;
  }

  public int getDeviceAddress() {
    return m_deviceAddress;
  }

  @Override
  public void close() {
    I2CJNI.i2CClose(m_port);
  }

  /**
   * Generic transaction.
   *
   * <p>This is a lower-level interface to the I2C hardware giving you more control over each
   * transaction. If you intend to write multiple bytes in the same transaction and do not plan to
   * receive anything back, use writeBulk() instead. Calling this with a receiveSize of 0 will
   * result in an error.
   *
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize Number of bytes to read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean transaction(
      byte[] dataToSend, int sendSize, byte[] dataReceived, int receiveSize) {
    if (dataToSend.length < sendSize) {
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + sendSize);
    }
    if (dataReceived.length < receiveSize) {
      throw new IllegalArgumentException(
          "dataReceived is too small, must be at least " + receiveSize);
    }
    return I2CJNI.i2CTransactionB(
            m_port,
            (byte) m_deviceAddress,
            dataToSend,
            (byte) sendSize,
            dataReceived,
            (byte) receiveSize)
        < 0;
  }

  /**
   * Generic transaction.
   *
   * <p>This is a lower-level interface to the I2C hardware giving you more control over each
   * transaction.
   *
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize Number of bytes to read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean transaction(
      ByteBuffer dataToSend, int sendSize, ByteBuffer dataReceived, int receiveSize) {
    if (dataToSend.hasArray() && dataReceived.hasArray()) {
      return transaction(dataToSend.array(), sendSize, dataReceived.array(), receiveSize);
    }
    if (!dataToSend.isDirect()) {
      throw new IllegalArgumentException("dataToSend must be a direct buffer");
    }
    if (dataToSend.capacity() < sendSize) {
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + sendSize);
    }
    if (!dataReceived.isDirect()) {
      throw new IllegalArgumentException("dataReceived must be a direct buffer");
    }
    if (dataReceived.capacity() < receiveSize) {
      throw new IllegalArgumentException(
          "dataReceived is too small, must be at least " + receiveSize);
    }

    return I2CJNI.i2CTransaction(
            m_port,
            (byte) m_deviceAddress,
            dataToSend,
            (byte) sendSize,
            dataReceived,
            (byte) receiveSize)
        < 0;
  }

  /**
   * Attempt to address a device on the I2C bus.
   *
   * <p>This allows you to figure out if there is a device on the I2C bus that responds to the
   * address specified in the constructor.
   *
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean addressOnly() {
    return transaction(new byte[0], (byte) 0, new byte[0], (byte) 0);
  }

  /**
   * Execute a write transaction with the device.
   *
   * <p>Write a single byte to a register on a device and wait until the transaction is complete.
   *
   * @param registerAddress The address of the register on the device to be written.
   * @param data The byte to write to the register on the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean write(int registerAddress, int data) {
    byte[] buffer = new byte[2];
    buffer[0] = (byte) registerAddress;
    buffer[1] = (byte) data;
    return I2CJNI.i2CWriteB(m_port, (byte) m_deviceAddress, buffer, (byte) buffer.length) < 0;
  }

  /**
   * Execute a write transaction with the device.
   *
   * <p>Write multiple bytes to a register on a device and wait until the transaction is complete.
   *
   * @param data The data to write to the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean writeBulk(byte[] data) {
    return writeBulk(data, data.length);
  }

  /**
   * Execute a write transaction with the device.
   *
   * <p>Write multiple bytes to a register on a device and wait until the transaction is complete.
   *
   * @param data The data to write to the device.
   * @param size The number of data bytes to write.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean writeBulk(byte[] data, int size) {
    if (data.length < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return I2CJNI.i2CWriteB(m_port, (byte) m_deviceAddress, data, (byte) size) < 0;
  }

  /**
   * Execute a write transaction with the device.
   *
   * <p>Write multiple bytes to a register on a device and wait until the transaction is complete.
   *
   * @param data The data to write to the device.
   * @param size The number of data bytes to write.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public synchronized boolean writeBulk(ByteBuffer data, int size) {
    if (data.hasArray()) {
      return writeBulk(data.array(), size);
    }
    if (!data.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (data.capacity() < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }

    return I2CJNI.i2CWrite(m_port, (byte) m_deviceAddress, data, (byte) size) < 0;
  }

  /**
   * Execute a read transaction with the device.
   *
   * <p>Read bytes from a device. Most I2C devices will auto-increment the register pointer
   * internally allowing you to read consecutive registers on a device in a single transaction.
   *
   * @param registerAddress The register to read first in the transaction.
   * @param count The number of bytes to read in the transaction.
   * @param buffer A pointer to the array of bytes to store the data read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean read(int registerAddress, int count, byte[] buffer) {
    requireNonNullParam(buffer, "buffer", "read");

    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count + " given");
    }
    if (buffer.length < count) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);
    }

    byte[] registerAddressArray = new byte[1];
    registerAddressArray[0] = (byte) registerAddress;

    return transaction(registerAddressArray, registerAddressArray.length, buffer, count);
  }

  private ByteBuffer m_readDataToSendBuffer;

  /**
   * Execute a read transaction with the device.
   *
   * <p>Read bytes from a device. Most I2C devices will auto-increment the register pointer
   * internally allowing you to read consecutive registers on a device in a single transaction.
   *
   * @param registerAddress The register to read first in the transaction.
   * @param count The number of bytes to read in the transaction.
   * @param buffer A buffer to store the data read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean read(int registerAddress, int count, ByteBuffer buffer) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count + " given");
    }

    if (buffer.hasArray()) {
      return read(registerAddress, count, buffer.array());
    }

    if (!buffer.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (buffer.capacity() < count) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);
    }

    synchronized (this) {
      if (m_readDataToSendBuffer == null) {
        m_readDataToSendBuffer = ByteBuffer.allocateDirect(1);
      }
      m_readDataToSendBuffer.put(0, (byte) registerAddress);

      return transaction(m_readDataToSendBuffer, 1, buffer, count);
    }
  }

  /**
   * Execute a read only transaction with the device.
   *
   * <p>Read bytes from a device. This method does not write any data to prompt the device.
   *
   * @param buffer A pointer to the array of bytes to store the data read from the device.
   * @param count The number of bytes to read in the transaction.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean readOnly(byte[] buffer, int count) {
    requireNonNullParam(buffer, "buffer", "readOnly");
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count + " given");
    }
    if (buffer.length < count) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);
    }

    return I2CJNI.i2CReadB(m_port, (byte) m_deviceAddress, buffer, (byte) count) < 0;
  }

  /**
   * Execute a read only transaction with the device.
   *
   * <p>Read bytes from a device. This method does not write any data to prompt the device.
   *
   * @param buffer A pointer to the array of bytes to store the data read from the device.
   * @param count The number of bytes to read in the transaction.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  public boolean readOnly(ByteBuffer buffer, int count) {
    if (count < 1) {
      throw new BoundaryException("Value must be at least 1, " + count + " given");
    }

    if (buffer.hasArray()) {
      return readOnly(buffer.array(), count);
    }

    if (!buffer.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (buffer.capacity() < count) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);
    }

    return I2CJNI.i2CRead(m_port, (byte) m_deviceAddress, buffer, (byte) count) < 0;
  }

  /*
   * Send a broadcast write to all devices on the I2C bus.
   *
   * <p>This is not currently implemented!
   *
   * @param registerAddress The register to write on all devices on the bus.
   * @param data            The value to write to the devices.
   */
  // public void broadcast(int registerAddress, int data) {
  // }

  /**
   * Verify that a device's registers contain expected values.
   *
   * <p>Most devices will have a set of registers that contain a known value that can be used to
   * identify them. This allows an I2C device driver to easily verify that the device contains the
   * expected value.
   *
   * @param registerAddress The base register to start reading from the device.
   * @param count The size of the field to be verified.
   * @param expected A buffer containing the values expected from the device.
   * @return true if the sensor was verified to be connected
   * @pre The device must support and be configured to use register auto-increment.
   */
  public boolean verifySensor(int registerAddress, int count, byte[] expected) {
    // TODO: Make use of all 7 read bytes
    byte[] dataToSend = new byte[1];

    byte[] deviceData = new byte[4];
    for (int i = 0; i < count; i += 4) {
      int toRead = count - i < 4 ? count - i : 4;
      // Read the chunk of data. Return false if the sensor does not
      // respond.
      dataToSend[0] = (byte) (registerAddress + i);
      if (transaction(dataToSend, 1, deviceData, toRead)) {
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
