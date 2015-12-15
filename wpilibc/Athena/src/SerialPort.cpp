/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SerialPort.h"

#include "HAL/HAL.hpp"
#include <stdarg.h>

// static ViStatus _VI_FUNCH ioCompleteHandler (ViSession vi, ViEventType
// eventType, ViEvent event, ViAddr userHandle);

/**
 * Create an instance of a Serial Port class.
 *
 * @param baudRate The baud rate to configure the serial port.
 * @param port The physical port to use
 * @param dataBits The number of data bits per transfer.  Valid values are
 * between 5 and 8 bits.
 * @param parity Select the type of parity checking to use.
 * @param stopBits The number of stop bits to use as defined by the enum
 * StopBits.
 */
SerialPort::SerialPort(uint32_t baudRate, Port port, uint8_t dataBits,
                       SerialPort::Parity parity, SerialPort::StopBits stopBits)
{
  int32_t status = 0;

  m_port = port;

  serialInitializePort(port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  serialSetBaudRate(port, baudRate, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  serialSetDataBits(port, dataBits, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  serialSetParity(port, parity, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  serialSetStopBits(port, stopBits, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  // Set the default timeout to 5 seconds.
  SetTimeout(5.0f);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  EnableTermination();

  // viInstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler,
  // this);
  // viEnableEvent(m_portHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_nullptr);

  HALReport(HALUsageReporting::kResourceType_SerialPort, 0);
}

/**
 * Destructor.
 */
SerialPort::~SerialPort() {
  int32_t status = 0;
  serialClose(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the type of flow control to enable on this port.
 *
 * By default, flow control is disabled.
 */
void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl) {
  int32_t status = 0;
  serialSetFlowControl(m_port, flowControl, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Enable termination and specify the termination character.
 *
 * Termination is currently only implemented for receive.
 * When the the terminator is recieved, the Read() or Scanf() will return
 *   fewer bytes than requested, stopping after the terminator.
 *
 * @param terminator The character to use for termination.
 */
void SerialPort::EnableTermination(char terminator) {
  int32_t status = 0;
  serialEnableTermination(m_port, terminator, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Disable termination behavior.
 */
void SerialPort::DisableTermination() {
  int32_t status = 0;
  serialDisableTermination(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of bytes currently available to read from the serial port.
 *
 * @return The number of bytes available to read
 */
int32_t SerialPort::GetBytesReceived() {
  int32_t status = 0;
  int32_t retVal = serialGetBytesReceived(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Read raw bytes out of the buffer.
 *
 * @param buffer Pointer to the buffer to store the bytes in.
 * @param count The maximum number of bytes to read.
 * @return The number of bytes actually read into the buffer.
 */
uint32_t SerialPort::Read(char *buffer, int32_t count) {
  int32_t status = 0;
  int32_t retVal = serialRead(m_port, buffer, count, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Write raw bytes to the buffer.
 *
 * @param buffer Pointer to the buffer to read the bytes from.
 * @param count The maximum number of bytes to write.
 * @return The number of bytes actually written into the port.
 */
uint32_t SerialPort::Write(const std::string &buffer, int32_t count) {
  int32_t status = 0;
  int32_t retVal = serialWrite(m_port, buffer.c_str(), count, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Configure the timeout of the serial port.
 *
 * This defines the timeout for transactions with the hardware.
 * It will affect reads and very large writes.
 *
 * @param timeout The number of seconds to to wait for I/O.
 */
void SerialPort::SetTimeout(float timeout) {
  int32_t status = 0;
  serialSetTimeout(m_port, timeout, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Specify the size of the input buffer.
 *
 * Specify the amount of data that can be stored before data
 * from the device is returned to Read or Scanf.  If you want
 * data that is recieved to be returned immediately, set this to 1.
 *
 * It the buffer is not filled before the read timeout expires, all
 * data that has been received so far will be returned.
 *
 * @param size The read buffer size.
 */
void SerialPort::SetReadBufferSize(uint32_t size) {
  int32_t status = 0;
  serialSetReadBufferSize(m_port, size, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Specify the size of the output buffer.
 *
 * Specify the amount of data that can be stored before being
 * transmitted to the device.
 *
 * @param size The write buffer size.
 */
void SerialPort::SetWriteBufferSize(uint32_t size) {
  int32_t status = 0;
  serialSetWriteBufferSize(m_port, size, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Specify the flushing behavior of the output buffer.
 *
 * When set to kFlushOnAccess, data is synchronously written to the serial port
 *   after each call to either Printf() or Write().
 *
 * When set to kFlushWhenFull, data will only be written to the serial port when
 *   the buffer is full or when Flush() is called.
 *
 * @param mode The write buffer mode.
 */
void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode) {
  int32_t status = 0;
  serialSetWriteMode(m_port, mode, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Force the output buffer to be written to the port.
 *
 * This is used when SetWriteBufferMode() is set to kFlushWhenFull to force a
 * flush before the buffer is full.
 */
void SerialPort::Flush() {
  int32_t status = 0;
  serialFlush(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Reset the serial port driver to a known state.
 *
 * Empty the transmit and receive buffers in the device and formatted I/O.
 */
void SerialPort::Reset() {
  int32_t status = 0;
  serialClear(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}
