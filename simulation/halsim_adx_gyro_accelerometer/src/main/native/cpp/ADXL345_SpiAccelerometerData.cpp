/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_SpiAccelerometerData.h"

#include <cstring>

#include <mockdata/SPIData.h>

using namespace hal;

const double ADXL345_SpiAccelerometer::LSB = 1 / 0.00390625;

static void ADXL345SPI_ReadBufferCallback(const char* name, void* param,
                                          uint8_t* buffer, uint32_t count) {
  ADXL345_SpiAccelerometer* sim = static_cast<ADXL345_SpiAccelerometer*>(param);
  sim->HandleRead(buffer, count);
}

static void ADXL345SPI_WriteBufferCallback(const char* name, void* param,
                                           const uint8_t* buffer,
                                           uint32_t count) {
  ADXL345_SpiAccelerometer* sim = static_cast<ADXL345_SpiAccelerometer*>(param);
  sim->HandleWrite(buffer, count);
}

ADXL345_SpiAccelerometer::ADXL345_SpiAccelerometer(int port) : m_port(port) {
  m_readCallbackId =
      HALSIM_RegisterSPIReadCallback(port, ADXL345SPI_ReadBufferCallback, this);
  m_writeCallbackId = HALSIM_RegisterSPIWriteCallback(
      port, ADXL345SPI_WriteBufferCallback, this);
}

ADXL345_SpiAccelerometer::~ADXL345_SpiAccelerometer() {
  HALSIM_CancelSPIReadCallback(m_port, m_readCallbackId);
  HALSIM_CancelSPIWriteCallback(m_port, m_writeCallbackId);
}

void ADXL345_SpiAccelerometer::HandleWrite(const uint8_t* buffer,
                                           uint32_t count) {
  m_lastWriteAddress = buffer[0] & 0xF;
}

void ADXL345_SpiAccelerometer::HandleRead(uint8_t* buffer, uint32_t count) {
  bool writeAll = count == 7;
  int byteIndex = 1;

  if (writeAll || m_lastWriteAddress == 0x02) {
    int16_t val = static_cast<int16_t>(GetX() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }

  if (writeAll || m_lastWriteAddress == 0x04) {
    int16_t val = static_cast<int16_t>(GetY() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }

  if (writeAll || m_lastWriteAddress == 0x06) {
    int16_t val = static_cast<int16_t>(GetZ() * LSB);
    std::memcpy(&buffer[byteIndex], &val, sizeof(val));
    byteIndex += sizeof(val);
  }
}
