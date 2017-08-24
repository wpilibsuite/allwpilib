/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "SensorBase.h"

enum HAL_I2CPort : int32_t;

namespace frc {

/**
 * I2C bus interface class.
 *
 * This class is intended to be used by sensor (and other I2C device) drivers.
 * It probably should not be used directly.
 *
 */
class I2C : SensorBase {
 public:
  enum Port { kOnboard = 0, kMXP };

  I2C(Port port, int deviceAddress);
  virtual ~I2C();

  I2C(const I2C&) = delete;
  I2C& operator=(const I2C&) = delete;

  bool Transaction(uint8_t* dataToSend, int sendSize, uint8_t* dataReceived,
                   int receiveSize);
  bool AddressOnly();
  bool Write(int registerAddress, uint8_t data);
  bool WriteBulk(uint8_t* data, int count);
  bool Read(int registerAddress, int count, uint8_t* data);
  bool ReadOnly(int count, uint8_t* buffer);
  // void Broadcast(int registerAddress, uint8_t data);
  bool VerifySensor(int registerAddress, int count, const uint8_t* expected);

 private:
  HAL_I2CPort m_port;
  int m_deviceAddress;
};

}  // namespace frc
