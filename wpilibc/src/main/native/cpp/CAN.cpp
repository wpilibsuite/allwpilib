// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/CAN.h"

#include <utility>

#include <hal/CAN.h>
#include <hal/CANAPI.h>
#include <hal/Errors.h>
#include <hal/UsageReporting.h>

#include "frc/Errors.h"

using namespace frc;

CAN::CAN(int busId, int deviceId)
    : CAN{busId, deviceId, kTeamManufacturer, kTeamDeviceType} {}

CAN::CAN(int busId, int deviceId, int deviceManufacturer, int deviceType) {
  int32_t status = 0;
  m_handle = HAL_InitializeCAN(
      busId, static_cast<HAL_CANManufacturer>(deviceManufacturer), deviceId,
      static_cast<HAL_CANDeviceType>(deviceType), &status);
  FRC_CheckErrorStatus(status, "device id {} mfg {} type {}", deviceId,
                       deviceManufacturer, deviceType);

  HAL_ReportUsage(
      fmt::format("CAN[{}][{}][{}]", deviceType, deviceManufacturer, deviceId),
      "");
}

void CAN::WritePacket(int apiId, const HAL_CANMessage& message) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, apiId, &message, &status);
  FRC_CheckErrorStatus(status, "WritePacket");
}

void CAN::WritePacketRepeating(int apiId, const HAL_CANMessage& message,
                               int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, apiId, &message, repeatMs, &status);
  FRC_CheckErrorStatus(status, "WritePacketRepeating");
}

void CAN::WriteRTRFrame(int apiId, const HAL_CANMessage& message) {
  int32_t status = 0;
  HAL_WriteCANRTRFrame(m_handle, apiId, &message, &status);
  FRC_CheckErrorStatus(status, "WriteRTRFrame");
}

int CAN::WritePacketNoError(int apiId, const HAL_CANMessage& message) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, apiId, &message, &status);
  return status;
}

int CAN::WritePacketRepeatingNoError(int apiId, const HAL_CANMessage& message,
                                     int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, apiId, &message, repeatMs, &status);
  return status;
}

int CAN::WriteRTRFrameNoError(int apiId, const HAL_CANMessage& message) {
  int32_t status = 0;
  HAL_WriteCANRTRFrame(m_handle, apiId, &message, &status);
  return status;
}

void CAN::StopPacketRepeating(int apiId) {
  int32_t status = 0;
  HAL_StopCANPacketRepeating(m_handle, apiId, &status);
  FRC_CheckErrorStatus(status, "StopPacketRepeating");
}

bool CAN::ReadPacketNew(int apiId, HAL_CANReceiveMessage* data) {
  int32_t status = 0;
  HAL_ReadCANPacketNew(m_handle, apiId, data, &status);
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

bool CAN::ReadPacketLatest(int apiId, HAL_CANReceiveMessage* data) {
  int32_t status = 0;
  HAL_ReadCANPacketLatest(m_handle, apiId, data, &status);
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

bool CAN::ReadPacketTimeout(int apiId, int timeoutMs,
                            HAL_CANReceiveMessage* data) {
  int32_t status = 0;
  HAL_ReadCANPacketTimeout(m_handle, apiId, data, timeoutMs, &status);
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
