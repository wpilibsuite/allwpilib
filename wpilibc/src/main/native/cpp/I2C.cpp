// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/I2C.h"

#include <algorithm>

#include <hal/FRCUsageReporting.h>
#include <hal/I2C.h>

#include "frc/Errors.h"

using namespace frc;

I2C::I2C(Port port, int deviceAddress)
    : m_port(static_cast<HAL_I2CPort>(port)), m_deviceAddress(deviceAddress) {
  int32_t status = 0;

  if (port == I2C::Port::kOnboard) {
    FRC_ReportWarning(
        "Onboard I2C port is subject to system lockups. See Known "
        "Issues page for "
        "details");
  }

  HAL_InitializeI2C(m_port, &status);
  FRC_CheckErrorStatus(status, "Port {}", static_cast<int>(port));

  HAL_Report(HALUsageReporting::kResourceType_I2C, deviceAddress);
}

I2C::Port I2C::GetPort() const {
  return static_cast<Port>(static_cast<int>(m_port));
}

int I2C::GetDeviceAddress() const {
  return m_deviceAddress;
}

bool I2C::Transaction(uint8_t* dataToSend, int sendSize, uint8_t* dataReceived,
                      int receiveSize) {
  int32_t status = 0;
  status = HAL_TransactionI2C(m_port, m_deviceAddress, dataToSend, sendSize,
                              dataReceived, receiveSize);
  return status < 0;
}

bool I2C::AddressOnly() {
  return Transaction(nullptr, 0, nullptr, 0);
}

bool I2C::Write(int registerAddress, uint8_t data) {
  uint8_t buffer[2];
  buffer[0] = registerAddress;
  buffer[1] = data;
  int32_t status = 0;
  status = HAL_WriteI2C(m_port, m_deviceAddress, buffer, sizeof(buffer));
  return status < 0;
}

bool I2C::WriteBulk(uint8_t* data, int count) {
  int32_t status = 0;
  status = HAL_WriteI2C(m_port, m_deviceAddress, data, count);
  return status < 0;
}

bool I2C::Read(int registerAddress, int count, uint8_t* buffer) {
  if (count < 1) {
    throw FRC_MakeError(err::ParameterOutOfRange, "count {}", count);
  }
  if (!buffer) {
    throw FRC_MakeError(err::NullParameter, "buffer");
  }
  uint8_t regAddr = registerAddress;
  return Transaction(&regAddr, sizeof(regAddr), buffer, count);
}

bool I2C::ReadOnly(int count, uint8_t* buffer) {
  if (count < 1) {
    throw FRC_MakeError(err::ParameterOutOfRange, "count {}", count);
  }
  if (!buffer) {
    throw FRC_MakeError(err::NullParameter, "buffer");
  }
  return HAL_ReadI2C(m_port, m_deviceAddress, buffer, count) < 0;
}

bool I2C::VerifySensor(int registerAddress, int count,
                       const uint8_t* expected) {
  // TODO: Make use of all 7 read bytes
  uint8_t deviceData[4];
  for (int i = 0, curRegisterAddress = registerAddress; i < count;
       i += 4, curRegisterAddress += 4) {
    int toRead = std::min(count - i, 4);
    // Read the chunk of data.  Return false if the sensor does not respond.
    if (Read(curRegisterAddress, toRead, deviceData)) {
      return false;
    }

    for (int j = 0; j < toRead; j++) {
      if (deviceData[j] != expected[i + j]) {
        return false;
      }
    }
  }
  return true;
}
