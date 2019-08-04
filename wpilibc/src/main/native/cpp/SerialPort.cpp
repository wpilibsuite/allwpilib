/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SerialPort.h"

#include <utility>

#include <hal/HAL.h>
#include <hal/SerialPort.h>

// static ViStatus _VI_FUNCH ioCompleteHandler (ViSession vi, ViEventType
// eventType, ViEvent event, ViAddr userHandle);

using namespace frc;

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

SerialPort::SerialPort(int baudRate, const wpi::Twine& portName, Port port,
                       int dataBits, SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  m_port = port;

  wpi::SmallVector<char, 64> buf;
  const char* portNameC = portName.toNullTerminatedStringRef(buf).data();

  HAL_InitializeSerialPortDirect(static_cast<HAL_SerialPort>(port), portNameC,
                                 &status);
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

SerialPort::~SerialPort() {
  int32_t status = 0;
  HAL_CloseSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

SerialPort::SerialPort(SerialPort&& rhs)
    : ErrorBase(std::move(rhs)),
      m_resourceManagerHandle(std::move(rhs.m_resourceManagerHandle)),
      m_portHandle(std::move(rhs.m_portHandle)),
      m_consoleModeEnabled(std::move(rhs.m_consoleModeEnabled)) {
  std::swap(m_port, rhs.m_port);
}

SerialPort& SerialPort::operator=(SerialPort&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  m_resourceManagerHandle = std::move(rhs.m_resourceManagerHandle);
  m_portHandle = std::move(rhs.m_portHandle);
  m_consoleModeEnabled = std::move(rhs.m_consoleModeEnabled);
  std::swap(m_port, rhs.m_port);

  return *this;
}

void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl) {
  int32_t status = 0;
  HAL_SetSerialFlowControl(static_cast<HAL_SerialPort>(m_port), flowControl,
                           &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::EnableTermination(char terminator) {
  int32_t status = 0;
  HAL_EnableSerialTermination(static_cast<HAL_SerialPort>(m_port), terminator,
                              &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::DisableTermination() {
  int32_t status = 0;
  HAL_DisableSerialTermination(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int SerialPort::GetBytesReceived() {
  int32_t status = 0;
  int retVal =
      HAL_GetSerialBytesReceived(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int SerialPort::Read(char* buffer, int count) {
  int32_t status = 0;
  int retVal = HAL_ReadSerial(static_cast<HAL_SerialPort>(m_port), buffer,
                              count, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int SerialPort::Write(const char* buffer, int count) {
  return Write(wpi::StringRef(buffer, static_cast<size_t>(count)));
}

int SerialPort::Write(wpi::StringRef buffer) {
  int32_t status = 0;
  int retVal = HAL_WriteSerial(static_cast<HAL_SerialPort>(m_port),
                               buffer.data(), buffer.size(), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

void SerialPort::SetTimeout(double timeout) {
  int32_t status = 0;
  HAL_SetSerialTimeout(static_cast<HAL_SerialPort>(m_port), timeout, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::SetReadBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(static_cast<HAL_SerialPort>(m_port), size,
                              &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::SetWriteBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(static_cast<HAL_SerialPort>(m_port), size,
                               &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode) {
  int32_t status = 0;
  HAL_SetSerialWriteMode(static_cast<HAL_SerialPort>(m_port), mode, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::Flush() {
  int32_t status = 0;
  HAL_FlushSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void SerialPort::Reset() {
  int32_t status = 0;
  HAL_ClearSerial(static_cast<HAL_SerialPort>(m_port), &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}
