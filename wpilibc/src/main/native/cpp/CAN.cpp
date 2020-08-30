/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/CAN.h"

#include <utility>

#include <hal/CAN.h>
#include <hal/CANAPI.h>
#include <hal/Errors.h>
#include <hal/FRCUsageReporting.h>

using namespace frc;

CAN::CAN(int deviceId) {
  int32_t status = 0;
  m_handle =
      HAL_InitializeCAN(kTeamManufacturer, deviceId, kTeamDeviceType, &status);
  if (status != 0) {
    wpi_setHALError(status);
    m_handle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_CAN, deviceId + 1);
}

CAN::CAN(int deviceId, int deviceManufacturer, int deviceType) {
  int32_t status = 0;
  m_handle = HAL_InitializeCAN(
      static_cast<HAL_CANManufacturer>(deviceManufacturer), deviceId,
      static_cast<HAL_CANDeviceType>(deviceType), &status);
  if (status != 0) {
    wpi_setHALError(status);
    m_handle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_CAN, deviceId + 1);
}

CAN::~CAN() {
  if (StatusIsFatal()) return;
  if (m_handle != HAL_kInvalidHandle) {
    HAL_CleanCAN(m_handle);
    m_handle = HAL_kInvalidHandle;
  }
}

void CAN::WritePacket(const uint8_t* data, int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, data, length, apiId, &status);
  wpi_setHALError(status);
}

void CAN::WritePacketRepeating(const uint8_t* data, int length, int apiId,
                               int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, data, length, apiId, repeatMs, &status);
  wpi_setHALError(status);
}

void CAN::WriteRTRFrame(int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANRTRFrame(m_handle, length, apiId, &status);
  wpi_setHALError(status);
}

int CAN::WritePacketNoError(const uint8_t* data, int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, data, length, apiId, &status);
  return status;
}

int CAN::WritePacketRepeatingNoError(const uint8_t* data, int length, int apiId,
                                     int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, data, length, apiId, repeatMs, &status);
  return status;
}

int CAN::WriteRTRFrameNoError(int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANRTRFrame(m_handle, length, apiId, &status);
  return status;
}

void CAN::StopPacketRepeating(int apiId) {
  int32_t status = 0;
  HAL_StopCANPacketRepeating(m_handle, apiId, &status);
  wpi_setHALError(status);
}

bool CAN::ReadPacketNew(int apiId, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketNew(m_handle, apiId, data->data, &data->length,
                       &data->timestamp, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setHALError(status);
    return false;
  } else {
    return true;
  }
}

bool CAN::ReadPacketLatest(int apiId, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketLatest(m_handle, apiId, data->data, &data->length,
                          &data->timestamp, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setHALError(status);
    return false;
  } else {
    return true;
  }
}

bool CAN::ReadPacketTimeout(int apiId, int timeoutMs, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketTimeout(m_handle, apiId, data->data, &data->length,
                           &data->timestamp, timeoutMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setHALError(status);
    return false;
  } else {
    return true;
  }
}
