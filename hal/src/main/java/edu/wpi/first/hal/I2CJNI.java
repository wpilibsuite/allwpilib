// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

/**
 * I2C HAL JNI functions.
 *
 * @see "I2C.h"
 */
public class I2CJNI extends JNIWrapper {
  /**
   * Initializes the I2C port.
   *
   * <p>Opens the port if necessary and saves the handle. If opening the MXP port, also sets up the
   * channel functions appropriately.
   *
   * @param port The port to open, 0 for the on-board, 1 for the MXP.
   * @see "HAL_InitializeI2C"
   */
  public static native void i2CInitialize(int port);

  /**
   * Generic I2C read/write transaction.
   *
   * <p>This is a lower-level interface to the I2C hardware giving you more control over each
   * transaction.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The address of the register on the device to be read/written.
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize Number of bytes to read from the device.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_TransactionI2C"
   */
  public static native int i2CTransaction(
      int port,
      byte address,
      ByteBuffer dataToSend,
      byte sendSize,
      ByteBuffer dataReceived,
      byte receiveSize);

  /**
   * Generic I2C read/write transaction.
   *
   * <p>This is a lower-level interface to the I2C hardware giving you more control over each
   * transaction.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The address of the register on the device to be read/written.
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param sendSize Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize Number of bytes to read from the device.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_TransactionI2C"
   */
  public static native int i2CTransactionB(
      int port,
      byte address,
      byte[] dataToSend,
      byte sendSize,
      byte[] dataReceived,
      byte receiveSize);

  /**
   * Executes a write transaction with the device.
   *
   * <p>Writes a single byte to a register on a device and wait until the transaction is complete.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The address of the register on the device to be written.
   * @param dataToSend The byte to write to the register on the device.
   * @param sendSize Number of bytes to send.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_WriteI2C"
   */
  public static native int i2CWrite(int port, byte address, ByteBuffer dataToSend, byte sendSize);

  /**
   * Executes a write transaction with the device.
   *
   * <p>Writes a single byte to a register on a device and wait until the transaction is complete.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The address of the register on the device to be written.
   * @param dataToSend The byte to write to the register on the device.
   * @param sendSize Number of bytes to send.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_WriteI2C"
   */
  public static native int i2CWriteB(int port, byte address, byte[] dataToSend, byte sendSize);

  /**
   * Executes a read transaction with the device.
   *
   * <p>Reads bytes from a device. Most I2C devices will auto-increment the register pointer
   * internally allowing you to read consecutive registers on a device in a single transaction.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The register to read first in the transaction.
   * @param dataReceived A ByteBuffer to store the data read from the device.
   * @param receiveSize The number of bytes to read in the transaction.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_ReadI2C"
   */
  public static native int i2CRead(
      int port, byte address, ByteBuffer dataReceived, byte receiveSize);

  /**
   * Executes a read transaction with the device.
   *
   * <p>Reads bytes from a device. Most I2C devices will auto-increment the register pointer
   * internally allowing you to read consecutive registers on a device in a single transaction.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @param address The register to read first in the transaction.
   * @param dataReceived A byte array to store the data read from the device.
   * @param receiveSize The number of bytes to read in the transaction.
   * @return &gt;= 0 on success or -1 on transfer abort.
   * @see "HAL_ReadI2C"
   */
  public static native int i2CReadB(int port, byte address, byte[] dataReceived, byte receiveSize);

  /**
   * Closes an I2C port.
   *
   * @param port The I2C port, 0 for the on-board, 1 for the MXP.
   * @see "HAL_CloseI2C"
   */
  public static native void i2CClose(int port);

  /** Utility class. */
  private I2CJNI() {}
}
