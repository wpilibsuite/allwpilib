/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_I2CAccelerometerData.h"

#include <cstring>

#include <mockdata/I2CData.h>

using namespace hal;

const double ADXL345_I2CData::LSB = 1 / 0.00390625;

static void ADXL345I2C_ReadBufferCallback(const char* name, void* param,
                                          uint8_t* buffer, uint32_t count) {
  ADXL345_I2CData* sim = static_cast<ADXL345_I2CData*>(param);
  sim->HandleRead(buffer, count);
}

static void ADXL345I2C_WriteBufferCallback(const char* name, void* param,
                                           const uint8_t* buffer,
                                           uint32_t count) {
  ADXL345_I2CData* sim = static_cast<ADXL345_I2CData*>(param);
  sim->HandleWrite(buffer, count);
}

ADXL345_I2CData::ADXL345_I2CData(int port) : m_port(port) {
  m_readCallbackId =
      HALSIM_RegisterI2CReadCallback(port, ADXL345I2C_ReadBufferCallback, this);
  m_writeCallbackId = HALSIM_RegisterI2CWriteCallback(
      port, ADXL345I2C_WriteBufferCallback, this);
}

ADXL345_I2CData::~ADXL345_I2CData() {
  HALSIM_CancelI2CReadCallback(m_port, m_readCallbackId);
  HALSIM_CancelI2CWriteCallback(m_port, m_writeCallbackId);
}

void ADXL345_I2CData::ADXL345_I2CData::HandleWrite(const uint8_t* buffer,
                                                   uint32_t count) {
  m_lastWriteAddress = buffer[0];
}

void ADXL345_I2CData::HandleRead(uint8_t* buffer, uint32_t count) {
  bool writeAll = count == 6;
  int byteIndex = 0;

  if (writeAll || m_lastWriteAddress == 0x32) {
    int16_t val = static_cast<int16_t>(GetX() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }

  if (writeAll || m_lastWriteAddress == 0x34) {
    int16_t val = static_cast<int16_t>(GetY() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }

  if (writeAll || m_lastWriteAddress == 0x36) {
    int16_t val = static_cast<int16_t>(GetZ() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }
}
