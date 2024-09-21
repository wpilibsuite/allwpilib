// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SerialPort.h"

#include <string>

#include <hal/FRCUsageReporting.h>
#include <hal/SerialPort.h>

#include "frc/Errors.h"

using namespace frc;

SerialPort::SerialPort(int baudRate, Port port, int dataBits,
                       SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  m_portHandle =
      HAL_InitializeSerialPort(static_cast<HAL_SerialPort>(port), &status);
  FRC_CheckErrorStatus(status, "Port {}", static_cast<int>(port));
  HAL_SetSerialBaudRate(m_portHandle, baudRate, &status);
  FRC_CheckErrorStatus(status, "SetSerialBaudRate {}", baudRate);
  HAL_SetSerialDataBits(m_portHandle, dataBits, &status);
  FRC_CheckErrorStatus(status, "SetSerialDataBits {}", dataBits);
  HAL_SetSerialParity(m_portHandle, parity, &status);
  FRC_CheckErrorStatus(status, "SetSerialParity {}", static_cast<int>(parity));
  HAL_SetSerialStopBits(m_portHandle, stopBits, &status);
  FRC_CheckErrorStatus(status, "SetSerialStopBits {}",
                       static_cast<int>(stopBits));

  // Set the default timeout to 5 seconds.
  SetTimeout(5_s);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  DisableTermination();

  HAL_Report(HALUsageReporting::kResourceType_SerialPort,
             static_cast<uint8_t>(port) + 1);
}

SerialPort::SerialPort(int baudRate, std::string_view portName, Port port,
                       int dataBits, SerialPort::Parity parity,
                       SerialPort::StopBits stopBits) {
  int32_t status = 0;

  m_portHandle =
      HAL_InitializeSerialPortDirect(static_cast<HAL_SerialPort>(port),
                                     std::string(portName).c_str(), &status);
  FRC_CheckErrorStatus(status, "Port {}", static_cast<int>(port));
  HAL_SetSerialBaudRate(m_portHandle, baudRate, &status);
  FRC_CheckErrorStatus(status, "SetSerialBaudRate {}", baudRate);
  HAL_SetSerialDataBits(m_portHandle, dataBits, &status);
  FRC_CheckErrorStatus(status, "SetSerialDataBits {}", dataBits);
  HAL_SetSerialParity(m_portHandle, parity, &status);
  FRC_CheckErrorStatus(status, "SetSerialParity {}", static_cast<int>(parity));
  HAL_SetSerialStopBits(m_portHandle, stopBits, &status);
  FRC_CheckErrorStatus(status, "SetSerialStopBits {}",
                       static_cast<int>(stopBits));

  // Set the default timeout to 5 seconds.
  SetTimeout(5_s);

  // Don't wait until the buffer is full to transmit.
  SetWriteBufferMode(kFlushOnAccess);

  DisableTermination();

  HAL_Report(HALUsageReporting::kResourceType_SerialPort,
             static_cast<uint8_t>(port) + 1);
}

void SerialPort::SetFlowControl(SerialPort::FlowControl flowControl) {
  int32_t status = 0;
  HAL_SetSerialFlowControl(m_portHandle, flowControl, &status);
  FRC_CheckErrorStatus(status, "SetFlowControl {}",
                       static_cast<int>(flowControl));
}

void SerialPort::EnableTermination(char terminator) {
  int32_t status = 0;
  HAL_EnableSerialTermination(m_portHandle, terminator, &status);
  FRC_CheckErrorStatus(status, "EnableTermination {}", terminator);
}

void SerialPort::DisableTermination() {
  int32_t status = 0;
  HAL_DisableSerialTermination(m_portHandle, &status);
  FRC_CheckErrorStatus(status, "DisableTermination");
}

int SerialPort::GetBytesReceived() {
  int32_t status = 0;
  int retVal = HAL_GetSerialBytesReceived(m_portHandle, &status);
  FRC_CheckErrorStatus(status, "GetBytesReceived");
  return retVal;
}

int SerialPort::Read(char* buffer, int count) {
  int32_t status = 0;
  int retVal = HAL_ReadSerial(m_portHandle, buffer, count, &status);
  FRC_CheckErrorStatus(status, "Read");
  return retVal;
}

int SerialPort::Write(const char* buffer, int count) {
  return Write(std::string_view(buffer, static_cast<size_t>(count)));
}

int SerialPort::Write(std::string_view buffer) {
  int32_t status = 0;
  int retVal =
      HAL_WriteSerial(m_portHandle, buffer.data(), buffer.size(), &status);
  FRC_CheckErrorStatus(status, "Write");
  return retVal;
}

void SerialPort::SetTimeout(units::second_t timeout) {
  int32_t status = 0;
  HAL_SetSerialTimeout(m_portHandle, timeout.value(), &status);
  FRC_CheckErrorStatus(status, "SetTimeout");
}

void SerialPort::SetReadBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialReadBufferSize(m_portHandle, size, &status);
  FRC_CheckErrorStatus(status, "SetReadBufferSize {}", size);
}

void SerialPort::SetWriteBufferSize(int size) {
  int32_t status = 0;
  HAL_SetSerialWriteBufferSize(m_portHandle, size, &status);
  FRC_CheckErrorStatus(status, "SetWriteBufferSize {}", size);
}

void SerialPort::SetWriteBufferMode(SerialPort::WriteBufferMode mode) {
  int32_t status = 0;
  HAL_SetSerialWriteMode(m_portHandle, mode, &status);
  FRC_CheckErrorStatus(status, "SetWriteBufferMode {}", static_cast<int>(mode));
}

void SerialPort::Flush() {
  int32_t status = 0;
  HAL_FlushSerial(m_portHandle, &status);
  FRC_CheckErrorStatus(status, "Flush");
}

void SerialPort::Reset() {
  int32_t status = 0;
  HAL_ClearSerial(m_portHandle, &status);
  FRC_CheckErrorStatus(status, "Reset");
}
