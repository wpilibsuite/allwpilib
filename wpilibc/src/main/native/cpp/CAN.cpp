// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/CAN.h"

#include <utility>

#include <hal/CAN.h>
#include <hal/CANAPI.h>
#include <hal/Errors.h>
#include <hal/FRCUsageReporting.h>

#include "frc/Errors.h"

using namespace frc;

CAN::CAN(int deviceId) {
  int32_t status = 0;
  m_handle =
      HAL_InitializeCAN(kTeamManufacturer, deviceId, kTeamDeviceType, &status);
  FRC_CheckErrorStatus(status, "device id {}", deviceId);

  HAL_Report(HALUsageReporting::kResourceType_CAN, deviceId + 1);
}

CAN::CAN(int deviceId, int deviceManufacturer, int deviceType) {
  int32_t status = 0;
  m_handle = HAL_InitializeCAN(
      static_cast<HAL_CANManufacturer>(deviceManufacturer), deviceId,
      static_cast<HAL_CANDeviceType>(deviceType), &status);
  FRC_CheckErrorStatus(status, "device id {} mfg {} type {}", deviceId,
                       deviceManufacturer, deviceType);

  HAL_Report(HALUsageReporting::kResourceType_CAN, deviceId + 1);
}

void CAN::WritePacket(const uint8_t* data, int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, data, length, apiId, &status);
  FRC_CheckErrorStatus(status, "WritePacket");
}

void CAN::WritePacketRepeating(const uint8_t* data, int length, int apiId,
                               int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, data, length, apiId, repeatMs, &status);
  FRC_CheckErrorStatus(status, "WritePacketRepeating");
}

void CAN::WriteRTRFrame(int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANRTRFrame(m_handle, length, apiId, &status);
  FRC_CheckErrorStatus(status, "WriteRTRFrame");
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
  FRC_CheckErrorStatus(status, "StopPacketRepeating");
}

bool CAN::ReadPacketNew(int apiId, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketNew(m_handle, apiId, data->data, &data->length,
                       &data->timestamp, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    FRC_CheckErrorStatus(status, "ReadPacketNew");
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
    FRC_CheckErrorStatus(status, "ReadPacketLatest");
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
    FRC_CheckErrorStatus(status, "ReadPacketTimeout");
    return false;
  } else {
    return true;
  }
}

uint64_t CAN::GetTimestampBaseTime() {
  return HAL_GetCANPacketBaseTime();
}
