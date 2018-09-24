/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/I2C.h"

#include <utility>

#include <hal/HAL.h>
#include <hal/I2C.h>

#include "frc/WPIErrors.h"

using namespace frc;

I2C::I2C(Port port, int deviceAddress)
    : m_port(static_cast<HAL_I2CPort>(port)), m_deviceAddress(deviceAddress) {
  int32_t status = 0;
  HAL_InitializeI2C(m_port, &status);
  // wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  HAL_Report(HALUsageReporting::kResourceType_I2C, deviceAddress);
}

I2C::~I2C() { HAL_CloseI2C(m_port); }

I2C::I2C(I2C&& rhs)
    : ErrorBase(std::move(rhs)),
      m_deviceAddress(std::move(rhs.m_deviceAddress)) {
  std::swap(m_port, rhs.m_port);
}

I2C& I2C::operator=(I2C&& rhs) {
  ErrorBase::operator=(std::move(rhs));

  std::swap(m_port, rhs.m_port);
  m_deviceAddress = std::move(rhs.m_deviceAddress);

  return *this;
}

bool I2C::Transaction(uint8_t* dataToSend, int sendSize, uint8_t* dataReceived,
                      int receiveSize) {
  int32_t status = 0;
  status = HAL_TransactionI2C(m_port, m_deviceAddress, dataToSend, sendSize,
                              dataReceived, receiveSize);
  // wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return status < 0;
}

bool I2C::AddressOnly() { return Transaction(nullptr, 0, nullptr, 0); }

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
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
    return true;
  }
  if (buffer == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "buffer");
    return true;
  }
  uint8_t regAddr = registerAddress;
  return Transaction(&regAddr, sizeof(regAddr), buffer, count);
}

bool I2C::ReadOnly(int count, uint8_t* buffer) {
  if (count < 1) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
    return true;
  }
  if (buffer == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "buffer");
    return true;
  }
  return HAL_ReadI2C(m_port, m_deviceAddress, buffer, count) < 0;
}

bool I2C::VerifySensor(int registerAddress, int count,
                       const uint8_t* expected) {
  // TODO: Make use of all 7 read bytes
  uint8_t deviceData[4];
  for (int i = 0, curRegisterAddress = registerAddress; i < count;
       i += 4, curRegisterAddress += 4) {
    int toRead = count - i < 4 ? count - i : 4;
    // Read the chunk of data.  Return false if the sensor does not respond.
    if (Read(curRegisterAddress, toRead, deviceData)) return false;

    for (int j = 0; j < toRead; j++) {
      if (deviceData[j] != expected[i + j]) return false;
    }
  }
  return true;
}
