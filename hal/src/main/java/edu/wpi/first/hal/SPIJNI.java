// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

/**
 * SPI HAL JNI functions.
 *
 * @see "SPI.h"
 */
public class SPIJNI extends JNIWrapper {
  /** Invalid port number. */
  public static final int INVALID_PORT = -1;

  /** Onboard SPI bus port CS0. */
  public static final int ONBOARD_CS0_PORT = 0;

  /** Onboard SPI bus port CS1. */
  public static final int ONBOARD_CS1_PORT = 1;

  /** Onboard SPI bus port CS2. */
  public static final int ONBOARD_CS2_PORT = 2;

  /** Onboard SPI bus port CS3. */
  public static final int ONBOARD_CS3_PORT = 3;

  /** MXP (roboRIO MXP) SPI bus port. */
  public static final int MXP_PORT = 4;

  /** Clock idle low, data sampled on rising edge. */
  public static final int SPI_MODE0 = 0;

  /** Clock idle low, data sampled on falling edge. */
  public static final int SPI_MODE1 = 1;

  /** Clock idle high, data sampled on falling edge. */
  public static final int SPI_MODE2 = 2;

  /** Clock idle high, data sampled on rising edge. */
  public static final int SPI_MODE3 = 3;

  /**
   * Initializes the SPI port. Opens the port if necessary and saves the handle.
   *
   * <p>If opening the MXP port, also sets up the channel functions appropriately.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS3, 4 for MXP
   * @see "HAL_InitializeSPI"
   */
  public static native void spiInitialize(int port);

  /**
   * Performs an SPI send/receive transaction.
   *
   * <p>This is a lower-level interface to the spi hardware giving you more control over each
   * transaction.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param size Number of bytes to transfer. [0..7]
   * @return Number of bytes transferred, -1 for error
   * @see "HAL_TransactionSPI"
   */
  public static native int spiTransaction(
      int port, ByteBuffer dataToSend, ByteBuffer dataReceived, byte size);

  /**
   * Performs an SPI send/receive transaction.
   *
   * <p>This is a lower-level interface to the spi hardware giving you more control over each
   * transaction.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param dataToSend Buffer of data to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param size Number of bytes to transfer. [0..7]
   * @return Number of bytes transferred, -1 for error
   * @see "HAL_TransactionSPI"
   */
  public static native int spiTransactionB(
      int port, byte[] dataToSend, byte[] dataReceived, byte size);

  /**
   * Executes a write transaction with the device.
   *
   * <p>Writes to a device and wait until the transaction is complete.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param dataToSend The data to write to the register on the device.
   * @param sendSize The number of bytes to be written
   * @return The number of bytes written. -1 for an error
   * @see "HAL_WriteSPI"
   */
  public static native int spiWrite(int port, ByteBuffer dataToSend, byte sendSize);

  /**
   * Executes a write transaction with the device.
   *
   * <p>Writes to a device and wait until the transaction is complete.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param dataToSend The data to write to the register on the device.
   * @param sendSize The number of bytes to be written
   * @return The number of bytes written. -1 for an error
   * @see "HAL_WriteSPI"
   */
  public static native int spiWriteB(int port, byte[] dataToSend, byte sendSize);

  /**
   * Executes a read from the device.
   *
   * <p>This method does not write any data out to the device.
   *
   * <p>Most spi devices will require a register address to be written before they begin returning
   * data.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param initiate initiates a transaction when true. Just reads when false.
   * @param dataReceived A pointer to the array of bytes to store the data read from the device.
   * @param size The number of bytes to read in the transaction. [1..7]
   * @return Number of bytes read. -1 for error.
   * @see "HAL_ReadSPI"
   */
  public static native int spiRead(int port, boolean initiate, ByteBuffer dataReceived, byte size);

  /**
   * Executes a read from the device.
   *
   * <p>This method does not write any data out to the device.
   *
   * <p>Most spi devices will require a register address to be written before they begin returning
   * data.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param initiate initiates a transaction when true. Just reads when false.
   * @param dataReceived A pointer to the array of bytes to store the data read from the device.
   * @param size The number of bytes to read in the transaction. [1..7]
   * @return Number of bytes read. -1 for error.
   * @see "HAL_ReadSPI"
   */
  public static native int spiReadB(int port, boolean initiate, byte[] dataReceived, byte size);

  /**
   * Closes the SPI port.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @see "HAL_CloseSPI"
   */
  public static native void spiClose(int port);

  /**
   * Sets the clock speed for the SPI bus.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param speed The speed in Hz (500KHz-10MHz)
   * @see "HAL_SetSPISpeed"
   */
  public static native void spiSetSpeed(int port, int speed);

  /**
   * Sets the SPI Mode.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @param mode The SPI mode to use
   * @see "HAL_SetSPIMode"
   */
  public static native void spiSetMode(int port, int mode);

  /**
   * Gets the SPI Mode.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @return The SPI mode currently set
   * @see "HAL_GetSPIMode"
   */
  public static native int spiGetMode(int port);

  /**
   * Sets the CS Active high for a SPI port.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @see "HAL_SetSPIChipSelectActiveHigh"
   */
  public static native void spiSetChipSelectActiveHigh(int port);

  /**
   * Sets the CS Active low for a SPI port.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP
   * @see "HAL_SetSPIChipSelectActiveLow"
   */
  public static native void spiSetChipSelectActiveLow(int port);

  /**
   * Initializes the SPI automatic accumulator.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param bufferSize The accumulator buffer size.
   * @see "HAL_InitSPIAuto"
   */
  public static native void spiInitAuto(int port, int bufferSize);

  /**
   * Frees an SPI automatic accumulator.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @see "HAL_FreeSPIAuto"
   */
  public static native void spiFreeAuto(int port);

  /**
   * Sets the period for automatic SPI accumulation.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param period The accumulation period (seconds).
   * @see "HAL_StartSPIAutoRate"
   */
  public static native void spiStartAutoRate(int port, double period);

  /**
   * Starts the auto SPI accumulator on a specific trigger.
   *
   * <p>Note that triggering on both rising and falling edges is a valid configuration.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param digitalSourceHandle The trigger source to use (Either HAL_AnalogTriggerHandle or
   *     HAL_DigitalHandle).
   * @param analogTriggerType The analog trigger type, if the source is an analog trigger.
   * @param triggerRising Trigger on the rising edge if true.
   * @param triggerFalling Trigger on the falling edge if true.
   * @see "HAL_StartSPIAutoTrigger"
   */
  public static native void spiStartAutoTrigger(
      int port,
      int digitalSourceHandle,
      int analogTriggerType,
      boolean triggerRising,
      boolean triggerFalling);

  /**
   * Stops an automatic SPI accumulation.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @see "HAL_StopSPIAuto"
   */
  public static native void spiStopAuto(int port);

  /**
   * Sets the data to be transmitted to the device to initiate a read.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param dataToSend Pointer to the data to send (Gets copied for continue use, so no need to keep
   *     alive).
   * @param zeroSize The number of zeros to send after the data.
   * @see "HAL_SetSPIAutoTransmitData"
   */
  public static native void spiSetAutoTransmitData(int port, byte[] dataToSend, int zeroSize);

  /**
   * Immediately forces an SPI read to happen.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @see "HAL_ForceSPIAutoRead"
   */
  public static native void spiForceAutoRead(int port);

  /**
   * Reads data received by the SPI accumulator. Each received data sequence consists of a timestamp
   * followed by the received data bytes, one byte per word (in the least significant byte). The
   * length of each received data sequence is the same as the combined dataSize + zeroSize set in
   * spiSetAutoTransmitData.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param buffer The buffer to store the data into.
   * @param numToRead The number of words to read.
   * @param timeout The read timeout (in seconds).
   * @return The number of words actually read.
   * @see "HAL_ReadSPIAutoReceivedData"
   */
  public static native int spiReadAutoReceivedData(
      int port, ByteBuffer buffer, int numToRead, double timeout);

  /**
   * Reads data received by the SPI accumulator. Each received data sequence consists of a timestamp
   * followed by the received data bytes, one byte per word (in the least significant byte). The
   * length of each received data sequence is the same as the combined dataSize + zeroSize set in
   * spiSetAutoTransmitData.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param buffer The buffer to store the data into.
   * @param numToRead The number of words to read.
   * @param timeout The read timeout (in seconds).
   * @return The number of words actually read.
   * @see "HAL_ReadSPIAutoReceivedData"
   */
  public static native int spiReadAutoReceivedData(
      int port, int[] buffer, int numToRead, double timeout);

  /**
   * Gets the count of how many SPI accumulations have been missed.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @return The number of missed accumulations.
   * @see "HAL_GetSPIAutoDroppedCount"
   */
  public static native int spiGetAutoDroppedCount(int port);

  /**
   * Configure the Auto SPI Stall time between reads.
   *
   * @param port The number of the port to use. 0-3 for Onboard CS0-CS2, 4 for MXP.
   * @param csToSclkTicks the number of ticks to wait before asserting the cs pin
   * @param stallTicks the number of ticks to stall for
   * @param pow2BytesPerRead the number of bytes to read before stalling
   * @see "HAL_ConfigureSPIAutoStall"
   */
  public static native void spiConfigureAutoStall(
      int port, int csToSclkTicks, int stallTicks, int pow2BytesPerRead);

  /** Utility class. */
  private SPIJNI() {}
}
