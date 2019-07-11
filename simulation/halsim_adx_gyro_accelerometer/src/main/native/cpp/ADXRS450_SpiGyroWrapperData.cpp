/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXRS450_SpiGyroWrapperData.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include <hal/HALBase.h>
#include <mockdata/SPIData.h>

#ifdef _WIN32
#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#endif

using namespace hal;

static constexpr double kSamplePeriod = 0.0005;

const double ADXRS450_SpiGyroWrapper::kAngleLsb = 1 / 0.0125 / kSamplePeriod;
const double ADXRS450_SpiGyroWrapper::kMaxAngleDeltaPerMessage = 0.1875;
const int ADXRS450_SpiGyroWrapper::kPacketSize = 4 + 1;  // +1 for timestamp

template <class T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
  return (std::max)(low, (std::min)(value, high));
}

static void ADXRS450SPI_ReadBufferCallback(const char* name, void* param,
                                           uint8_t* buffer, uint32_t count) {
  auto sim = static_cast<ADXRS450_SpiGyroWrapper*>(param);
  sim->HandleRead(buffer, count);
}

static void ADXRS450SPI_ReadAutoReceivedData(const char* name, void* param,
                                             uint32_t* buffer,
                                             int32_t numToRead,
                                             int32_t* outputCount) {
  auto sim = static_cast<ADXRS450_SpiGyroWrapper*>(param);
  sim->HandleAutoReceiveData(buffer, numToRead, *outputCount);
}

ADXRS450_SpiGyroWrapper::ADXRS450_SpiGyroWrapper(int port) : m_port(port) {
  m_readCallbackId = HALSIM_RegisterSPIReadCallback(
      port, ADXRS450SPI_ReadBufferCallback, this);
  m_autoReceiveReadCallbackId = HALSIM_RegisterSPIReadAutoReceivedDataCallback(
      port, ADXRS450SPI_ReadAutoReceivedData, this);
}

ADXRS450_SpiGyroWrapper::~ADXRS450_SpiGyroWrapper() {
  HALSIM_CancelSPIReadCallback(m_port, m_readCallbackId);
  HALSIM_CancelSPIReadAutoReceivedDataCallback(m_port,
                                               m_autoReceiveReadCallbackId);
}
bool ADXRS450_SpiGyroWrapper::GetInitialized() const {
  return HALSIM_GetSPIInitialized(m_port);
}

void ADXRS450_SpiGyroWrapper::ResetData() {
  std::scoped_lock lock(m_angle.GetMutex());
  m_angle.Reset(0.0);
  m_angleDiff = 0;
}

void ADXRS450_SpiGyroWrapper::HandleRead(uint8_t* buffer, uint32_t count) {
  int returnCode = 0x00400AE0;
  std::memcpy(&buffer[0], &returnCode, sizeof(returnCode));
}

void ADXRS450_SpiGyroWrapper::HandleAutoReceiveData(uint32_t* buffer,
                                                    int32_t numToRead,
                                                    int32_t& outputCount) {
  std::scoped_lock lock(m_angle.GetMutex());
  int32_t messagesToSend =
      1 + std::abs(m_angleDiff > 0
                       ? std::ceil(m_angleDiff / kMaxAngleDeltaPerMessage)
                       : std::floor(m_angleDiff / kMaxAngleDeltaPerMessage));

  // Zero gets passed in during the "How much data do I need to read" step.
  // Else it is actually reading the accumulator
  if (numToRead == 0) {
    outputCount = messagesToSend * kPacketSize;
    return;
  }

  int valuesToRead = numToRead / kPacketSize;
  std::memset(&buffer[0], 0, numToRead * sizeof(uint32_t));

  int32_t status = 0;
  uint32_t timestamp = HAL_GetFPGATime(&status);

  for (int msgCtr = 0; msgCtr < valuesToRead; ++msgCtr) {
    // force the first message to be a rate of 0 to init the timestamp
    double cappedDiff = (msgCtr == 0)
                            ? 0
                            : clamp(m_angleDiff, -kMaxAngleDeltaPerMessage,
                                    kMaxAngleDeltaPerMessage);

    // first word is timestamp
    buffer[msgCtr * kPacketSize] = timestamp;

    int32_t valueToSend =
        ((static_cast<int32_t>(cappedDiff * kAngleLsb) << 10) & (~0x0C00000E)) |
        0x04000000;

    // following words have byte in LSB, in big endian order
    for (int i = 4; i >= 1; --i) {
      buffer[msgCtr * kPacketSize + i] =
          static_cast<uint32_t>(valueToSend) & 0xffu;
      valueToSend >>= 8;
    }

    m_angleDiff -= cappedDiff;
    timestamp += kSamplePeriod * 1e6;  // fpga time is in us
  }
}

void ADXRS450_SpiGyroWrapper::SetAngle(double angle) {
  std::scoped_lock lock(m_angle.GetMutex());
  if (m_angle != angle) {
    m_angleDiff += angle - m_angle;
    m_angle = angle;
  }
}
