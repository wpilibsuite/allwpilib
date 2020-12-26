// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SerialPort.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/SerialPort.h>

using namespace frc;

SerialPort::SerialPort(int baudRate, Port port, int dataBits,
                       SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  m_portHandle =
      HAL_InitializeSerialPort(static_cast<HAL_SerialPort>(port), &status);
  wpi_setHALError(status);
  // Don't continue if initialization failed
  if (status < 0) return;
  HAL_SetSerialBaudRate(m_portHandle, baudRate, &status);
  wpi_setHALError(status);
  HAL_SetSerialDataBits(m_portHandle, dataBits, &status);
  wpi_setHALError(status);
  HAL_SetSerialParity(m_portHandle, parity, &status);
  wpi_setHALError(status);
  HAL_SetSerialStopBits(m_portHandle, stopBits, &status);
  wpi_setHALError(status);

  // Set the default timeout to 5 seconds.
  SetTimeout(5.0);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  DisableTermination();

  HAL_Report(HALUsageReporting::kResourceType_SerialPort,
             static_cast<uint8_t>(port) + 1);
}

SerialPort::SerialPort(int baudRate, const wpi::Twine& portName, Port port,
                       int dataBits, SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  wpi::SmallVector<char, 64> buf;
  const char* portNameC = portName.toNullTerminatedStringRef(buf).data();

  m_portHandle = HAL_InitializeSerialPortDirect(
      static_cast<HAL_SerialPort>(port), portNameC, &status);
  wpi_setHALError(status);
  // Don't continue if initialization failed
  if (status < 0) return;
  HAL_SetSerialBaudRate(m_portHandle, baudRate, &status);
  wpi_setHALError(status);
  HAL_SetSerialDataBits(m_portHandle, dataBits, &status);
  wpi_setHALError(status);
  HAL_SetSerialParity(m_portHandle, parity, &status);
  wpi_setHALError(status);
  HAL_SetSerialStopBits(m_portHandle, stopBits, &status);
  wpi_setHALError(status);

  // Set the default timeout to 5 seconds.
  SetTimeout(5.0);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  DisableTermination();

  HAL_Report(HALUsageReporting::kResourceType_SerialPort,
             static_cast<uint8_t>(port) + 1);
}

SerialPort::~SerialPort() {
  int32_t status = 0;
  HAL_CloseSerial(m_portHandle, &status);
  wpi_setHALError(status);
}

void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl) {
  int32_t status = 0;
  HAL_SetSerialFlowControl(m_portHandle, flowControl, &status);
  wpi_setHALError(status);
}

void SerialPort::EnableTermination(char terminator) {
  int32_t status = 0;
  HAL_EnableSerialTermination(m_portHandle, terminator, &status);
  wpi_setHALError(status);
}

void SerialPort::DisableTermination() {
  int32_t status = 0;
  HAL_DisableSerialTermination(m_portHandle, &status);
  wpi_setHALError(status);
}

int SerialPort::GetBytesReceived() {
  int32_t status = 0;
  int retVal = HAL_GetSerialBytesReceived(m_portHandle, &status);
  wpi_setHALError(status);
  return retVal;
}

int SerialPort::Read(char* buffer, int count) {
  int32_t status = 0;
  int retVal = HAL_ReadSerial(m_portHandle, buffer, count, &status);
  wpi_setHALError(status);
  return retVal;
}

int SerialPort::Write(const char* buffer, int count) {
  return Write(wpi::StringRef(buffer, static_cast<size_t>(count)));
}

int SerialPort::Write(wpi::StringRef buffer) {
  int32_t status = 0;
  int retVal =
      HAL_WriteSerial(m_portHandle, buffer.data(), buffer.size(), &status);
  wpi_setHALError(status);
  return retVal;
}

void SerialPort::SetTimeout(double timeout) {
  int32_t status = 0;
  HAL_SetSerialTimeout(m_portHandle, timeout, &status);
  wpi_setHALError(status);
}

void SerialPort::SetReadBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(m_portHandle, size, &status);
  wpi_setHALError(status);
}

void SerialPort::SetWriteBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(m_portHandle, size, &status);
  wpi_setHALError(status);
}

void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode) {
  int32_t status = 0;
  HAL_SetSerialWriteMode(m_portHandle, mode, &status);
  wpi_setHALError(status);
}

void SerialPort::Flush() {
  int32_t status = 0;
  HAL_FlushSerial(m_portHandle, &status);
  wpi_setHALError(status);
}

void SerialPort::Reset() {
  int32_t status = 0;
  HAL_ClearSerial(m_portHandle, &status);
  wpi_setHALError(status);
}
