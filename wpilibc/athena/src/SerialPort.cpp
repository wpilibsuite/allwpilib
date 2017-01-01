/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/SerialPort.h"
#include "SerialPort.h"

#include "HAL/HAL.h"

// static ViStatus _VI_FUNCH ioCompleteHandler (ViSession vi, ViEventType
// eventType, ViEvent event, ViAddr userHandle);

using namespace frc;

/**
 * Create an instance of a Serial Port class.
 *
 * @param baudRate The baud rate to configure the serial port.
 * @param port     The physical port to use
 * @param dataBits The number of data bits per transfer.  Valid values are
 *                 between 5 and 8 bits.
 * @param parity   Select the type of parity checking to use.
 * @param stopBits The number of stop bits to use as defined by the enum
 *                 StopBits.
 */
SerialPort::SerialPort(int baudRate, Port port, int dataBits,
                       SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  m_port = port;

  HAL_InitializeSerialPort(static_cast<HAL_SerialPort>(port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  // Don't continue if initialization failed
  if (status < 0) return;
  HAL_SetSerialBaudRate(static_cast<HAL_SerialPort>(port), baudRate, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  HAL_SetSerialDataBits(static_cast<HAL_SerialPort>(port), dataBits, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  HAL_SetSerialParity(static_cast<HAL_SerialPort>(port), parity, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  HAL_SetSerialStopBits(static_cast<HAL_SerialPort>(port), stopBits, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  // Set the default timeout to 5 seconds.
  SetTimeout(5.0);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  EnableTermination();

  // viInstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler,
  // this);
  // viEnableEvent(m_portHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_NULL);

  HAL_Report(HALUsageReporting::kResourceType_SerialPort, 0);
}

/**
 * Destructor.
 */
SerialPort::~SerialPort() {
  int32_t status = 0;
  HAL_CloseSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the type of flow control to enable on this port.
 *
 * By default, flow control is disabled.
 */
void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl) {
  int32_t status = 0;
  HAL_SetSerialFlowControl(static_cast<HAL_SerialPort>(m_port), flowControl,
                           &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Enable termination and specify the termination character.
 *
 * Termination is currently only implemented for receive.
 * When the the terminator is recieved, the Read() or Scanf() will return
 * fewer bytes than requested, stopping after the terminator.
 *
 * @param terminator The character to use for termination.
 */
void SerialPort::EnableTermination(char terminator) {
  int32_t status = 0;
  HAL_EnableSerialTermination(static_cast<HAL_SerialPort>(m_port), terminator,
                              &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Disable termination behavior.
 */
void SerialPort::DisableTermination() {
  int32_t status = 0;
  HAL_DisableSerialTermination(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Get the number of bytes currently available to read from the serial port.
 *
 * @return The number of bytes available to read
 */
int SerialPort::GetBytesReceived() {
  int32_t status = 0;
  int retVal =
      HAL_GetSerialBytesReceived(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Read raw bytes out of the buffer.
 *
 * @param buffer Pointer to the buffer to store the bytes in.
 * @param count  The maximum number of bytes to read.
 * @return The number of bytes actually read into the buffer.
 */
int SerialPort::Read(char* buffer, int count) {
  int32_t status = 0;
  int retVal = HAL_ReadSerial(static_cast<HAL_SerialPort>(m_port), buffer,
                              count, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Write raw bytes to the buffer. Deprecated, please use StringRef overload. Use
 * Write({data, len}) to get a buffer that is shorter then the length of the
 * std::string.
 *
 * @param buffer Pointer to the buffer to read the bytes from. If string.size()
 * is less then count, only the length of string.size() will be sent.
 * @param count  The maximum number of bytes to write.
 * @return The number of bytes actually written into the port.
 */
int SerialPort::Write(const std::string& buffer, int count) {
  return Write(llvm::StringRef(
      buffer.data(), std::min(static_cast<int>(buffer.size()), count)));
}

/**
 * Write raw bytes to the buffer.
 *
 * @param buffer Pointer to the buffer to read the bytes from.
 * @param count  The maximum number of bytes to write.
 * @return The number of bytes actually written into the port.
 */
int SerialPort::Write(const char* buffer, int count) {
  return Write(llvm::StringRef(buffer, static_cast<size_t>(count)));
}

/**
 * Write raw bytes to the buffer.
 *
 * @param buffer StringRef to the buffer to read the bytes from.
 * @return The number of bytes actually written into the port.
 */
int SerialPort::Write(llvm::StringRef buffer) {
  int32_t status = 0;
  int retVal = HAL_WriteSerial(static_cast<HAL_SerialPort>(m_port),
                               buffer.data(), buffer.size(), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
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
void SerialPort::SetTimeout(double timeout) {
  int32_t status = 0;
  HAL_SetSerialTimeout(static_cast<HAL_SerialPort>(m_port), timeout, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
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
void SerialPort::SetReadBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(static_cast<HAL_SerialPort>(m_port), size,
                              &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Specify the size of the output buffer.
 *
 * Specify the amount of data that can be stored before being
 * transmitted to the device.
 *
 * @param size The write buffer size.
 */
void SerialPort::SetWriteBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(static_cast<HAL_SerialPort>(m_port), size,
                               &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Specify the flushing behavior of the output buffer.
 *
 * When set to kFlushOnAccess, data is synchronously written to the serial port
 * after each call to either Printf() or Write().
 *
 * When set to kFlushWhenFull, data will only be written to the serial port when
 * the buffer is full or when Flush() is called.
 *
 * @param mode The write buffer mode.
 */
void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode) {
  int32_t status = 0;
  HAL_SetSerialWriteMode(static_cast<HAL_SerialPort>(m_port), mode, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Force the output buffer to be written to the port.
 *
 * This is used when SetWriteBufferMode() is set to kFlushWhenFull to force a
 * flush before the buffer is full.
 */
void SerialPort::Flush() {
  int32_t status = 0;
  HAL_FlushSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Reset the serial port driver to a known state.
 *
 * Empty the transmit and receive buffers in the device and formatted I/O.
 */
void SerialPort::Reset() {
  int32_t status = 0;
  HAL_ClearSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}
