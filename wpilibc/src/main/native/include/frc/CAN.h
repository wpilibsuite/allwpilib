// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/CANAPITypes.h>

#include "frc/ErrorBase.h"

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
  /**
   * Create a new CAN communication interface with the specific device ID.
   * This uses the team manufacturer and device types.
   * The device ID is 6 bits (0-63)
   *
   * @param deviceId The device id
   */
  explicit CAN(int deviceId);

  /**
   * Create a new CAN communication interface with a specific device ID,
   * manufacturer and device type. The device ID is 6 bits, the
   * manufacturer is 8 bits, and the device type is 5 bits.
   *
   * @param deviceId           The device ID
   * @param deviceManufacturer The device manufacturer
   * @param deviceType         The device type
   */
  CAN(int deviceId, int deviceManufacturer, int deviceType);

  /**
   * Closes the CAN communication.
   */
  ~CAN() override;

  CAN(CAN&&) = default;
  CAN& operator=(CAN&&) = default;

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param data The data to write (8 bytes max)
   * @param length The data length to write
   * @param apiId The API ID to write.
   */
  void WritePacket(const uint8_t* data, int length, int apiId);

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is
   * 10 bits. The RoboRIO will automatically repeat the packet at the specified
   * interval
   *
   * @param data The data to write (8 bytes max)
   * @param length The data length to write
   * @param apiId The API ID to write.
   * @param repeatMs The period to repeat the packet at.
   */
  void WritePacketRepeating(const uint8_t* data, int length, int apiId,
                            int repeatMs);

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10
   * bits. The length by spec must match what is returned by the responding
   * device
   *
   * @param length The length to request (0 to 8)
   * @param apiId The API ID to write.
   */
  void WriteRTRFrame(int length, int apiId);

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param data The data to write (8 bytes max)
   * @param length The data length to write
   * @param apiId The API ID to write.
   */
  int WritePacketNoError(const uint8_t* data, int length, int apiId);

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is
   * 10 bits. The RoboRIO will automatically repeat the packet at the specified
   * interval
   *
   * @param data The data to write (8 bytes max)
   * @param length The data length to write
   * @param apiId The API ID to write.
   * @param repeatMs The period to repeat the packet at.
   */
  int WritePacketRepeatingNoError(const uint8_t* data, int length, int apiId,
                                  int repeatMs);

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10
   * bits. The length by spec must match what is returned by the responding
   * device
   *
   * @param length The length to request (0 to 8)
   * @param apiId The API ID to write.
   */
  int WriteRTRFrameNoError(int length, int apiId);

  /**
   * Stop a repeating packet with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to stop repeating
   */
  void StopPacketRepeating(int apiId);

  /**
   * Read a new CAN packet. This will only return properly once per packet
   * received. Multiple calls without receiving another packet will return
   * false.
   *
   * @param apiId The API ID to read.
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  bool ReadPacketNew(int apiId, CANData* data);

  /**
   * Read a CAN packet. The will continuously return the last packet received,
   * without accounting for packet age.
   *
   * @param apiId The API ID to read.
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  bool ReadPacketLatest(int apiId, CANData* data);

  /**
   * Read a CAN packet. The will return the last packet received until the
   * packet is older then the requested timeout. Then it will return false.
   *
   * @param apiId The API ID to read.
   * @param timeoutMs The timeout time for the packet
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  bool ReadPacketTimeout(int apiId, int timeoutMs, CANData* data);

  static constexpr HAL_CANManufacturer kTeamManufacturer = HAL_CAN_Man_kTeamUse;
  static constexpr HAL_CANDeviceType kTeamDeviceType =
      HAL_CAN_Dev_kMiscellaneous;

 private:
  hal::Handle<HAL_CANHandle> m_handle;
};
}  // namespace frc
