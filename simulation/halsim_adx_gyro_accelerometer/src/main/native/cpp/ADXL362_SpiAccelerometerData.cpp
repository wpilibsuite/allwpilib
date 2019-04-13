/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL362_SpiAccelerometerData.h"

#include <cstring>

#include <mockdata/SPIData.h>

using namespace hal;

const double ADXL362_SpiAccelerometer::LSB = 1 / 0.001;

static void ADXL362SPI_ReadBufferCallback(const char* name, void* param,
                                          uint8_t* buffer, uint32_t count) {
  ADXL362_SpiAccelerometer* sim = static_cast<ADXL362_SpiAccelerometer*>(param);
  sim->HandleRead(buffer, count);
}

static void ADXL362SPI_WriteBufferCallback(const char* name, void* param,
                                           const uint8_t* buffer,
                                           uint32_t count) {
  ADXL362_SpiAccelerometer* sim = static_cast<ADXL362_SpiAccelerometer*>(param);
  sim->HandleWrite(buffer, count);
}

ADXL362_SpiAccelerometer::ADXL362_SpiAccelerometer(int port) : m_port(port) {
  m_readCallbackId =
      HALSIM_RegisterSPIReadCallback(port, ADXL362SPI_ReadBufferCallback, this);
  m_writeCallbackId = HALSIM_RegisterSPIWriteCallback(
      port, ADXL362SPI_WriteBufferCallback, this);
}

ADXL362_SpiAccelerometer::~ADXL362_SpiAccelerometer() {
  HALSIM_CancelSPIReadCallback(m_port, m_readCallbackId);
  HALSIM_CancelSPIWriteCallback(m_port, m_writeCallbackId);
}
bool ADXL362_SpiAccelerometer::GetInitialized() const {
  return HALSIM_GetSPIInitialized(m_port);
}

void ADXL362_SpiAccelerometer::HandleWrite(const uint8_t* buffer,
                                           uint32_t count) {
  m_lastWriteAddress = buffer[1];
}

void ADXL362_SpiAccelerometer::HandleRead(uint8_t* buffer, uint32_t count) {
  // Init check
  if (m_lastWriteAddress == 0x02) {
    uint32_t numToPut = 0xF20000;
    std::memcpy(&buffer[0], &numToPut, sizeof(numToPut));
  } else {
    // Get Accelerations
    bool writeAll = count == 8;
    int byteIndex = 2;

    if (writeAll || m_lastWriteAddress == 0x0E) {
      int16_t val = static_cast<int16_t>(GetX() * LSB);
      std::memcpy(&buffer[byteIndex], &val, sizeof(val));
      byteIndex += sizeof(val);
    }

    if (writeAll || m_lastWriteAddress == 0x10) {
      int16_t val = static_cast<int16_t>(GetY() * LSB);
      std::memcpy(&buffer[byteIndex], &val, sizeof(val));
      byteIndex += sizeof(val);
    }

    if (writeAll || m_lastWriteAddress == 0x12) {
      int16_t val = static_cast<int16_t>(GetZ() * LSB);
      std::memcpy(&buffer[byteIndex], &val, sizeof(val));
      byteIndex += sizeof(val);
    }
  }
}
