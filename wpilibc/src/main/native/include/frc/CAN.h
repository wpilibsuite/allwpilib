// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/CANAPI.h>

namespace frc {

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
class CAN {
 public:
  /**
   * Create a new CAN communication interface with the specific device ID.
   * This uses the team manufacturer and device types.
   * The device ID is 6 bits (0-63)
   *
   * @param busId    The bus id
   * @param deviceId The device id
   */
  CAN(int busId, int deviceId);

  /**
   * Create a new CAN communication interface with a specific device ID,
   * manufacturer and device type. The device ID is 6 bits, the
   * manufacturer is 8 bits, and the device type is 5 bits.
   *
   * @param busId              The bus id
   * @param deviceId           The device ID
   * @param deviceManufacturer The device manufacturer
   * @param deviceType         The device type
   */
  CAN(int busId, int deviceId, int deviceManufacturer, int deviceType);

  CAN(CAN&&) = default;
  CAN& operator=(CAN&&) = default;

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   */
  void WritePacket(int apiId, const HAL_CANMessage& message);

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is
   * 10 bits. The RoboRIO will automatically repeat the packet at the specified
   * interval
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   * @param repeatMs The period to repeat the packet at.
   */
  void WritePacketRepeating(int apiId, const HAL_CANMessage& message,
                            int repeatMs);

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10
   * bits. The length by spec must match what is returned by the responding
   * device
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   */
  void WriteRTRFrame(int apiId, const HAL_CANMessage& message);

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   */
  int WritePacketNoError(int apiId, const HAL_CANMessage& message);

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is
   * 10 bits. The RoboRIO will automatically repeat the packet at the specified
   * interval
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   * @param repeatMs The period to repeat the packet at.
   */
  int WritePacketRepeatingNoError(int apiId, const HAL_CANMessage& message,
                                  int repeatMs);

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10
   * bits. The length by spec must match what is returned by the responding
   * device
   *
   * @param apiId The API ID to write.
   * @param message the CAN message.
   */
  int WriteRTRFrameNoError(int apiId, const HAL_CANMessage& message);

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
  bool ReadPacketNew(int apiId, HAL_CANReceiveMessage* data);

  /**
   * Read a CAN packet. The will continuously return the last packet received,
   * without accounting for packet age.
   *
   * @param apiId The API ID to read.
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  bool ReadPacketLatest(int apiId, HAL_CANReceiveMessage* data);

  /**
   * Read a CAN packet. The will return the last packet received until the
   * packet is older then the requested timeout. Then it will return false.
   *
   * @param apiId The API ID to read.
   * @param timeoutMs The timeout time for the packet
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  bool ReadPacketTimeout(int apiId, int timeoutMs, HAL_CANReceiveMessage* data);

  /// Team manufacturer.
  static constexpr HAL_CANManufacturer kTeamManufacturer = HAL_CAN_Man_kTeamUse;

  /// Team device type.
  static constexpr HAL_CANDeviceType kTeamDeviceType =
      HAL_CAN_Dev_kMiscellaneous;

 private:
  hal::Handle<HAL_CANHandle, HAL_CleanCAN> m_handle;
};
}  // namespace frc
