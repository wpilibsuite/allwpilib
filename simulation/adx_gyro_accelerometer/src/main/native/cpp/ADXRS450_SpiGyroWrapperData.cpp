/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstring>

#include "ADXRS450_SpiGyroWrapperData.h"
#include "MockData/SPIData.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

const double ADXRS450_SpiGyroWrapper::ANGLE_LSB = 1 / 0.0125 / 0.001;

static void ADXRS450SPI_ReadBufferCallback(const char* name, void* param,
                                           uint8_t* buffer, uint32_t count) {
  ADXRS450_SpiGyroWrapper* sim = static_cast<ADXRS450_SpiGyroWrapper*>(param);
  sim->HandleRead(buffer, count);
}

ADXRS450_SpiGyroWrapper::ADXRS450_SpiGyroWrapper(int port) : m_port(port) {
  HALSIM_RegisterSPIReadCallback(port, ADXRS450SPI_ReadBufferCallback, this);
}

ADXRS450_SpiGyroWrapper::~ADXRS450_SpiGyroWrapper() {}

void ADXRS450_SpiGyroWrapper::ResetData() {
  m_angle = 0;
  m_angleCallbacks = nullptr;
}

void ADXRS450_SpiGyroWrapper::HandleRead(uint8_t* buffer, uint32_t count) {
  int returnCode = 0x00400AE0;
  std::memcpy(&buffer[0], &returnCode, sizeof(returnCode));
}

int32_t ADXRS450_SpiGyroWrapper::RegisterAngleCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<std::mutex> lock(m_registerMutex);
    m_angleCallbacks =
        RegisterCallback(m_angleCallbacks, "Angle", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetAngle());
    callback("Angle", param, &value);
  }
  return newUid;
}
void ADXRS450_SpiGyroWrapper::CancelAngleCallback(int32_t uid) {
  m_angleCallbacks = CancelCallback(m_angleCallbacks, uid);
}
void ADXRS450_SpiGyroWrapper::InvokeAngleCallback(HAL_Value value) {
  InvokeCallback(m_angleCallbacks, "Angle", &value);
}
double ADXRS450_SpiGyroWrapper::GetAngle() { return m_angle; }
void ADXRS450_SpiGyroWrapper::SetAngle(double angle) {
  int32_t oldValue = m_angle.exchange(angle);
  if (oldValue != angle) {
    InvokeAngleCallback(MakeDouble(angle));

    long accumValue = angle * ANGLE_LSB;
    HALSIM_SetSPISetAccumulatorValue(m_port,accumValue);
  }
}
