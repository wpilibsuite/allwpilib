/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CAN.h"

#include <HAL/HAL.h>

using namespace frc;

/**
 * Create a new CAN communication interface with the specific device ID.
 * The device ID is 6 bits (0-63)
 */
CAN::CAN(int deviceId) {
  int32_t status = 0;
  m_handle =
      HAL_InitializeCAN(kTeamManufacturer, deviceId, kTeamDeviceType, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_handle = HAL_kInvalidHandle;
    return;
  }

  // HAL_Report(HALUsageReporting::kResourceType_CAN, deviceId);
}

/**
 * Closes the CAN communication.
 */
CAN::~CAN() {
  if (StatusIsFatal()) return;
  if (m_handle != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_CleanCAN(m_handle);
    m_handle = HAL_kInvalidHandle;
  }
}

/**
 * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
 *
 * @param data The data to write (8 bytes max)
 * @param length The data length to write
 * @param apiId The API ID to write.
 */
void CAN::WritePacket(const uint8_t* data, int length, int apiId) {
  int32_t status = 0;
  HAL_WriteCANPacket(m_handle, data, length, apiId, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Write a repeating packet to the CAN device with a specific ID. This ID is 10
 * bits. The RoboRIO will automatically repeat the packet at the specified
 * interval
 *
 * @param data The data to write (8 bytes max)
 * @param length The data length to write
 * @param apiId The API ID to write.
 * @param repeatMs The period to repeat the packet at.
 */
void CAN::WritePacketRepeating(const uint8_t* data, int length, int apiId,
                               int repeatMs) {
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(m_handle, data, length, apiId, repeatMs, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Stop a repeating packet with a specific ID. This ID is 10 bits.
 *
 * @param apiId The API ID to stop repeating
 */
void CAN::StopPacketRepeating(int apiId) {
  int32_t status = 0;
  HAL_StopCANPacketRepeating(m_handle, apiId, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Read a new CAN packet. This will only return properly once per packet
 * received. Multiple calls without receiving another packet will return false.
 *
 * @param apiId The API ID to read.
 * @param data Storage for the received data.
 * @return True if the data is valid, otherwise false.
 */
bool CAN::ReadPacketNew(int apiId, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketNew(m_handle, apiId, data->data, &data->length,
                       &data->timestamp, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    return false;
  } else {
    return true;
  }
}

/**
 * Read a CAN packet. The will continuously return the last packet received,
 * without accounting for packet age.
 *
 * @param apiId The API ID to read.
 * @param data Storage for the received data.
 * @return True if the data is valid, otherwise false.
 */
bool CAN::ReadPacketLatest(int apiId, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketLatest(m_handle, apiId, data->data, &data->length,
                          &data->timestamp, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    return false;
  } else {
    return true;
  }
}

/**
 * Read a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return false.
 *
 * @param apiId The API ID to read.
 * @param timeoutMs The timeout time for the packet
 * @param data Storage for the received data.
 * @return True if the data is valid, otherwise false.
 */
bool CAN::ReadPacketTimeout(int apiId, int timeoutMs, CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPacketTimeout(m_handle, apiId, data->data, &data->length,
                           &data->timestamp, timeoutMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    return false;
  } else {
    return true;
  }
}

/**
 * Read a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return false.
 * The period parameter is used when you know the packet is sent at specific
 * intervals, so calls will not attempt to read a new packet from the
 * network until that period has passed. We do not recommend users use this
 * API unless they know the implications.
 *
 * @param apiId The API ID to read.
 * @param timeoutMs The timeout time for the packet
 * @param periodMs The usual period for the packet
 * @param data Storage for the received data.
 * @return True if the data is valid, otherwise false.
 */
bool CAN::ReadPeriodicPacket(int apiId, int timeoutMs, int periodMs,
                             CANData* data) {
  int32_t status = 0;
  HAL_ReadCANPeriodicPacket(m_handle, apiId, data->data, &data->length,
                            &data->timestamp, timeoutMs, periodMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    return false;
  } else {
    return true;
  }
}
