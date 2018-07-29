/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_serialport Serial Port Functions
 * @ingroup hal_capi
 * @{
 */

// clang-format off
HAL_ENUM(HAL_SerialPort) {
  HAL_SerialPort_Onboard = 0,
  HAL_SerialPort_MXP = 1,
  HAL_SerialPort_USB1 = 2,
  HAL_SerialPort_USB2 = 3
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a serial port.
 *
 * The channels are either the onboard RS232, the mxp uart, or 2 USB ports. The
 * top port is USB1, the bottom port is USB2.
 *
 * @param port the serial port to initialize
 */
void HAL_InitializeSerialPort(HAL_SerialPort port, int32_t* status);

/**
 * Initializes a serial port with a direct name.
 *
 * This name is the VISA name for a specific port (find this in the web dash).
 * Note these are not always consistent between roboRIO reboots.
 *
 * @param port     the serial port to initialize
 * @param portName the VISA port name
 */
void HAL_InitializeSerialPortDirect(HAL_SerialPort port, const char* portName,
                                    int32_t* status);

/**
 * Sets the baud rate of a serial port.
 *
 * Any value between 0 and 0xFFFFFFFF may be used. Default is 9600.
 *
 * @param port the serial port
 * @param baud the baud rate to set
 */
void HAL_SetSerialBaudRate(HAL_SerialPort port, int32_t baud, int32_t* status);

/**
 * Sets the number of data bits on a serial port.
 *
 * Defaults to 8.
 *
 * @param port the serial port
 * @param bits the number of data bits (5-8)
 */
void HAL_SetSerialDataBits(HAL_SerialPort port, int32_t bits, int32_t* status);

/**
 * Sets the number of parity bits on a serial port.
 *
 * Valid values are:
 *   0: None (default)
 *   1: Odd
 *   2: Even
 *   3: Mark - Means exists and always 1
 *   4: Space - Means exists and always 0
 *
 * @param port   the serial port
 * @param parity the parity bit mode (see remarks for valid values)
 */
void HAL_SetSerialParity(HAL_SerialPort port, int32_t parity, int32_t* status);

/**
 * Sets the number of stop bits on a serial port.
 *
 * Valid values are:
 *   10: One stop bit (default)
 *   15: One and a half stop bits
 *   20: Two stop bits
 *
 * @param port     the serial port
 * @param stopBits the stop bit value (see remarks for valid values)
 */
void HAL_SetSerialStopBits(HAL_SerialPort port, int32_t stopBits,
                           int32_t* status);

/**
 * Sets the write mode on a serial port.
 *
 * Valid values are:
 *   1: Flush on access
 *   2: Flush when full (default)
 *
 * @param port the serial port
 * @param mode the mode to set (see remarks for valid values)
 */
void HAL_SetSerialWriteMode(HAL_SerialPort port, int32_t mode, int32_t* status);

/**
 * Sets the flow control mode of a serial port.
 *
 * Valid values are:
 *   0: None (default)
 *   1: XON-XOFF
 *   2: RTS-CTS
 *   3: DTR-DSR
 *
 * @param port the serial port
 * @param flow the mode to set (see remarks for valid values)
 */
void HAL_SetSerialFlowControl(HAL_SerialPort port, int32_t flow,
                              int32_t* status);

/**
 * Sets the minimum serial read timeout of a port.
 *
 * @param port    the serial port
 * @param timeout the timeout in milliseconds
 */
void HAL_SetSerialTimeout(HAL_SerialPort port, double timeout, int32_t* status);

/**
 * Sets the termination character that terminates a read.
 *
 * By default this is disabled.
 *
 * @param port       the serial port
 * @param terminator the termination character to set
 */
void HAL_EnableSerialTermination(HAL_SerialPort port, char terminator,
                                 int32_t* status);

/**
 * Disables a termination character for reads.
 *
 * @param port the serial port
 */
void HAL_DisableSerialTermination(HAL_SerialPort port, int32_t* status);

/**
 * Sets the size of the read buffer.
 *
 * @param port the serial port
 * @param size the read buffer size
 */
void HAL_SetSerialReadBufferSize(HAL_SerialPort port, int32_t size,
                                 int32_t* status);

/**
 * Sets the size of the write buffer.
 *
 * @param port the serial port
 * @param size the write buffer size
 */
void HAL_SetSerialWriteBufferSize(HAL_SerialPort port, int32_t size,
                                  int32_t* status);

/**
 * Gets the number of bytes currently in the read buffer.
 *
 * @param port the serial port
 * @return     the number of bytes in the read buffer
 */
int32_t HAL_GetSerialBytesReceived(HAL_SerialPort port, int32_t* status);

/**
 * Reads data from the serial port.
 *
 * Will wait for either timeout (if set), the termination char (if set), or the
 * count to be full. Whichever one comes first.
 *
 * @param port  the serial port
 * @param count the number of bytes maximum to read
 * @return      the number of bytes actually read
 */
int32_t HAL_ReadSerial(HAL_SerialPort port, char* buffer, int32_t count,
                       int32_t* status);

/**
 * Writes data to the serial port.
 *
 * @param port   the serial port
 * @param buffer the buffer to write
 * @param count  the number of bytes to write from the buffer
 * @return       the number of bytes actually written
 */
int32_t HAL_WriteSerial(HAL_SerialPort port, const char* buffer, int32_t count,
                        int32_t* status);

/**
 * Flushes the serial write buffer out to the port.
 *
 * @param port the serial port
 */
void HAL_FlushSerial(HAL_SerialPort port, int32_t* status);

/**
 * Clears the receive buffer of the serial port.
 *
 * @param port the serial port
 */
void HAL_ClearSerial(HAL_SerialPort port, int32_t* status);

/**
 * Closes a serial port.
 *
 * @param port the serial port to close
 */
void HAL_CloseSerial(HAL_SerialPort port, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
