/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/CANAPI.h>
#include <wpi/ArrayRef.h>

#include "ErrorBase.h"

namespace frc {
struct CANData {
  uint8_t data[8];
  int32_t length;
  uint64_t timestamp;
};

/**
 * High level class for interfacing with CAN devices conforming to
 * the standard CAN spec.
 *
 * No packets that can be sent gets blocked by the RoboRIO, so all methods
 * work identically in all robot modes.
 *
 * All methods are thread save, however the buffer objects passed in
 * by the user need to not be modified for the duration of their calls.
 */
class CAN : public ErrorBase {
 public:
  explicit CAN(int deviceId);
  ~CAN() override;

  void WritePacket(const uint8_t* data, int length, int apiId);
  void WritePacketRepeating(const uint8_t* data, int length, int apiId,
                            int repeatMs);
  void StopPacketRepeating(int apiId);

  bool ReadPacketNew(int apiId, CANData* data);
  bool ReadPacketLatest(int apiId, CANData* data);
  bool ReadPacketTimeout(int apiId, int timeoutMs, CANData* data);
  bool ReadPeriodicPacket(int apiId, int timeoutMs, int periodMs,
                          CANData* data);

  static constexpr HAL_CANManufacturer TeamManufacturer = HAL_CAN_Man_kTeamUse;
  static constexpr HAL_CANDeviceType TeamDeviceType =
      HAL_CAN_Dev_kMiscellaneous;

 private:
  HAL_CANHandle m_handle{HAL_kInvalidHandle};
};
}  // namespace frc
