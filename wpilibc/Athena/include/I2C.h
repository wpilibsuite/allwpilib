/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SensorBase.h"

/**
 * I2C bus interface class.
 *
 * This class is intended to be used by sensor (and other I2C device) drivers.
 * It probably should not be used directly.
 *
 */
class I2C : SensorBase {
 public:
  enum Port { kOnboard, kMXP };

  I2C(Port port, uint8_t deviceAddress);
  virtual ~I2C();

  I2C(const I2C&) = delete;
  I2C& operator=(const I2C&) = delete;

  bool Transaction(uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived,
                   uint8_t receiveSize);
  bool AddressOnly();
  bool Write(uint8_t registerAddress, uint8_t data);
  bool WriteBulk(uint8_t *data, uint8_t count);
  bool Read(uint8_t registerAddress, uint8_t count, uint8_t *data);
  bool ReadOnly(uint8_t count, uint8_t *buffer);
  void Broadcast(uint8_t registerAddress, uint8_t data);
  bool VerifySensor(uint8_t registerAddress, uint8_t count,
                    const uint8_t *expected);

 private:
  Port m_port;
  uint8_t m_deviceAddress;
};
