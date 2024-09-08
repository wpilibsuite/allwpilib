// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_serialport Serial Port Functions
 * @ingroup hal_capi
 * @{
 */

HAL_ENUM(HAL_SerialPort) {
  HAL_SerialPort_Onboard = 0,
  HAL_SerialPort_MXP = 1,
  HAL_SerialPort_USB1 = 2,
  HAL_SerialPort_USB2 = 3
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a serial port.
 *
 * The channels are either the onboard RS232, the MXP UART, or 2 USB ports. The
 * top port is USB1, the bottom port is USB2.
 *
 * @param[in] port the serial port to initialize
 * @param[out] status the error code, or 0 for success
 * @return Serial Port Handle
 */
HAL_SerialPortHandle HAL_InitializeSerialPort(HAL_SerialPort port,
                                              int32_t* status);

/**
 * Initializes a serial port with a direct name.
 *
 * This name is the /dev name for a specific port.
 * Note these are not always consistent between roboRIO reboots.
 *
 * @param[in] port     the serial port to initialize
 * @param[in] portName the dev port name
 * @param[out] status  the error code, or 0 for success
 * @return Serial Port Handle
 */
HAL_SerialPortHandle HAL_InitializeSerialPortDirect(HAL_SerialPort port,
                                                    const char* portName,
                                                    int32_t* status);

/**
 * Gets the raw serial port file descriptor from a handle.
 *
 * @param[in] handle the serial port handle
 * @param[out] status the error code, or 0 for success
 * @return the raw port descriptor
 */
int HAL_GetSerialFD(HAL_SerialPortHandle handle, int32_t* status);

/**
 * Sets the baud rate of a serial port.
 *
 * Any value between 0 and 0xFFFFFFFF may be used. Default is 9600.
 *
 * @param[in] handle  the serial port handle
 * @param[in] baud    the baud rate to set
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialBaudRate(HAL_SerialPortHandle handle, int32_t baud,
                           int32_t* status);

/**
 * Sets the number of data bits on a serial port.
 *
 * Defaults to 8.
 *
 * @param[in] handle  the serial port handle
 * @param[in] bits    the number of data bits (5-8)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialDataBits(HAL_SerialPortHandle handle, int32_t bits,
                           int32_t* status);

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
 * @param[in] handle  the serial port handle
 * @param[in] parity  the parity bit mode (see remarks for valid values)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialParity(HAL_SerialPortHandle handle, int32_t parity,
                         int32_t* status);

/**
 * Sets the number of stop bits on a serial port.
 *
 * Valid values are:
 *   10: One stop bit (default)
 *   15: One and a half stop bits
 *   20: Two stop bits
 *
 * @param[in] handle    the serial port handle
 * @param[in] stopBits  the stop bit value (see remarks for valid values)
 * @param[out] status   the error code, or 0 for success
 */
void HAL_SetSerialStopBits(HAL_SerialPortHandle handle, int32_t stopBits,
                           int32_t* status);

/**
 * Sets the write mode on a serial port.
 *
 * Valid values are:
 *   1: Flush on access
 *   2: Flush when full (default)
 *
 * @param[in] handle  the serial port handle
 * @param[in] mode    the mode to set (see remarks for valid values)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialWriteMode(HAL_SerialPortHandle handle, int32_t mode,
                            int32_t* status);

/**
 * Sets the flow control mode of a serial port.
 *
 * Valid values are:
 *   0: None (default)
 *   1: XON-XOFF
 *   2: RTS-CTS
 *   3: DTR-DSR
 *
 * @param[in] handle  the serial port handle
 * @param[in] flow    the mode to set (see remarks for valid values)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialFlowControl(HAL_SerialPortHandle handle, int32_t flow,
                              int32_t* status);

/**
 * Sets the minimum serial read timeout of a port.
 *
 * @param[in] handle   the serial port handle
 * @param[in] timeout  the timeout in milliseconds
 * @param[out] status  the error code, or 0 for success
 */
void HAL_SetSerialTimeout(HAL_SerialPortHandle handle, double timeout,
                          int32_t* status);

/**
 * Sets the termination character that terminates a read.
 *
 * By default this is disabled.
 *
 * @param[in] handle      the serial port handle
 * @param[in] terminator  the termination character to set
 * @param[out] status     the error code, or 0 for success
 */
void HAL_EnableSerialTermination(HAL_SerialPortHandle handle, char terminator,
                                 int32_t* status);

/**
 * Disables a termination character for reads.
 *
 * @param[in] handle  the serial port handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_DisableSerialTermination(HAL_SerialPortHandle handle, int32_t* status);

/**
 * Sets the size of the read buffer.
 *
 * @param[in] handle  the serial port handle
 * @param[in] size    the read buffer size
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialReadBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                 int32_t* status);

/**
 * Sets the size of the write buffer.
 *
 * @param[in] handle  the serial port handle
 * @param[in] size    the write buffer size
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetSerialWriteBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                  int32_t* status);

/**
 * Gets the number of bytes currently in the read buffer.
 *
 * @param[in] handle  the serial port handle
 * @param[out] status the error code, or 0 for success
 * @return the number of bytes in the read buffer
 */
int32_t HAL_GetSerialBytesReceived(HAL_SerialPortHandle handle,
                                   int32_t* status);

/**
 * Reads data from the serial port.
 *
 * Will wait for either timeout (if set), the termination char (if set), or the
 * count to be full. Whichever one comes first.
 *
 * @param[in] handle  the serial port handle
 * @param[out] buffer the buffer in which to store bytes read
 * @param[in] count   the number of bytes maximum to read
 * @param[out] status the error code, or 0 for success
 * @return the number of bytes actually read
 */
int32_t HAL_ReadSerial(HAL_SerialPortHandle handle, char* buffer, int32_t count,
                       int32_t* status);

/**
 * Writes data to the serial port.
 *
 * @param[in] handle  the serial port handle
 * @param[in] buffer  the buffer to write
 * @param[in] count   the number of bytes to write from the buffer
 * @param[out] status the error code, or 0 for success
 * @return the number of bytes actually written
 */
int32_t HAL_WriteSerial(HAL_SerialPortHandle handle, const char* buffer,
                        int32_t count, int32_t* status);

/**
 * Flushes the serial write buffer out to the port.
 *
 * @param[in] handle  the serial port handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_FlushSerial(HAL_SerialPortHandle handle, int32_t* status);

/**
 * Clears the receive buffer of the serial port.
 *
 * @param[in] handle  the serial port handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_ClearSerial(HAL_SerialPortHandle handle, int32_t* status);

/**
 * Closes a serial port.
 *
 * @param[in] handle  the serial port handle to close
 */
void HAL_CloseSerial(HAL_SerialPortHandle handle);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
