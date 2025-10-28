// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Serial Port JNI HAL functions.
 *
 * @see "SerialPort.h"
 */
public class SerialPortJNI extends JNIWrapper {
  /**
   * Initializes a serial port.
   *
   * <p>The channels are either the onboard RS232, the MXP UART, or 2 USB ports. The top port is
   * USB1, the bottom port is USB2.
   *
   * @param port the serial port to initialize
   * @return Serial Port Handle
   * @see "HAL_InitializeSerialPort"
   */
  public static native int serialInitializePort(byte port);

  /**
   * Initializes a serial port with a direct name.
   *
   * <p>This name is the /dev name for a specific port. Note these are not always consistent between
   * roboRIO reboots.
   *
   * @param port the serial port to initialize
   * @param portName the dev port name
   * @return Serial Port Handle
   * @see "HAL_InitializeSerialPortDirect"
   */
  public static native int serialInitializePortDirect(byte port, String portName);

  /**
   * Sets the baud rate of a serial port.
   *
   * <p>Any value between 0 and 0xFFFFFFFF may be used. Default is 9600.
   *
   * @param handle the serial port handle
   * @param baud the baud rate to set
   * @see "HAL_SetSerialBaudRate"
   */
  public static native void serialSetBaudRate(int handle, int baud);

  /**
   * Sets the number of data bits on a serial port.
   *
   * <p>Defaults to 8.
   *
   * @param handle the serial port handle
   * @param bits the number of data bits (5-8)
   * @see "HAL_SetSerialDataBits"
   */
  public static native void serialSetDataBits(int handle, byte bits);

  /**
   * Sets the number of parity bits on a serial port.
   *
   * <p>Valid values are: 0: None (default) 1: Odd 2: Even 3: Mark - Means exists and always 1 4:
   * Space - Means exists and always 0
   *
   * @param handle the serial port handle
   * @param parity the parity bit mode (see remarks for valid values)
   * @see "HAL_SetSerialParity"
   */
  public static native void serialSetParity(int handle, byte parity);

  /**
   * Sets the number of stop bits on a serial port.
   *
   * <p>Valid values are: 10: One stop bit (default) 15: One and a half stop bits 20: Two stop bits
   *
   * @param handle the serial port handle
   * @param stopBits the stop bit value (see remarks for valid values)
   * @see "HAL_SetSerialStopBits"
   */
  public static native void serialSetStopBits(int handle, byte stopBits);

  /**
   * Sets the write mode on a serial port.
   *
   * <p>Valid values are: 1: Flush on access 2: Flush when full (default)
   *
   * @param handle the serial port handle
   * @param mode the mode to set (see remarks for valid values)
   * @see "HAL_SetSerialWriteMode"
   */
  public static native void serialSetWriteMode(int handle, byte mode);

  /**
   * Sets the flow control mode of a serial port.
   *
   * <p>Valid values are: 0: None (default) 1: XON-XOFF 2: RTS-CTS 3: DTR-DSR
   *
   * @param handle the serial port handle
   * @param flow the mode to set (see remarks for valid values)
   * @see "HAL_SetSerialFlowControl"
   */
  public static native void serialSetFlowControl(int handle, byte flow);

  /**
   * Sets the minimum serial read timeout of a port.
   *
   * @param handle the serial port handle
   * @param timeout the timeout in milliseconds
   * @see "HAL_SetSerialTimeout"
   */
  public static native void serialSetTimeout(int handle, double timeout);

  /**
   * Sets the termination character that terminates a read.
   *
   * <p>By default this is disabled.
   *
   * @param handle the serial port handle
   * @param terminator the termination character to set
   * @see "HAL_EnableSerialTermination"
   */
  public static native void serialEnableTermination(int handle, char terminator);

  /**
   * Disables a termination character for reads.
   *
   * @param handle the serial port handle
   * @see "HAL_DisableSerialTermination"
   */
  public static native void serialDisableTermination(int handle);

  /**
   * Sets the size of the read buffer.
   *
   * @param handle the serial port handle
   * @param size the read buffer size
   * @see "HAL_SetSerialReadBufferSize"
   */
  public static native void serialSetReadBufferSize(int handle, int size);

  /**
   * Sets the size of the write buffer.
   *
   * @param handle the serial port handle
   * @param size the write buffer size
   * @see "HAL_SetSerialWriteBufferSize"
   */
  public static native void serialSetWriteBufferSize(int handle, int size);

  /**
   * Gets the number of bytes currently in the read buffer.
   *
   * @param handle the serial port handle
   * @return the number of bytes in the read buffer
   * @see "HAL_GetSerialBytesReceived"
   */
  public static native int serialGetBytesReceived(int handle);

  /**
   * Reads data from the serial port.
   *
   * <p>Will wait for either timeout (if set), the termination char (if set), or the count to be
   * full. Whichever one comes first.
   *
   * @param handle the serial port handle
   * @param buffer the buffer in which to store bytes read
   * @param count the number of bytes maximum to read
   * @return the number of bytes actually read
   * @see "HAL_ReadSerial"
   */
  public static native int serialRead(int handle, byte[] buffer, int count);

  /**
   * Writes data to the serial port.
   *
   * @param handle the serial port handle
   * @param buffer the buffer to write
   * @param count the number of bytes to write from the buffer
   * @return the number of bytes actually written
   * @see "HAL_WriteSerial"
   */
  public static native int serialWrite(int handle, byte[] buffer, int count);

  /**
   * Flushes the serial write buffer out to the port.
   *
   * @param handle the serial port handle
   * @see "HAL_FlushSerial"
   */
  public static native void serialFlush(int handle);

  /**
   * Clears the receive buffer of the serial port.
   *
   * @param handle the serial port handle
   * @see "HAL_ClearSerial"
   */
  public static native void serialClear(int handle);

  /**
   * Closes a serial port.
   *
   * @param handle the serial port handle to close
   * @see "HAL_CloseSerial"
   */
  public static native void serialClose(int handle);

  /** Utility class. */
  private SerialPortJNI() {}
}
