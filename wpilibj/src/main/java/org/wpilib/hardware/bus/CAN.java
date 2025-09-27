// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.bus;

import java.io.Closeable;
import org.wpilib.hardware.hal.CANAPIJNI;
import org.wpilib.hardware.hal.CANAPITypes;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.can.CANReceiveMessage;

/**
 * High level class for interfacing with CAN devices conforming to the standard CAN spec.
 *
 * <p>No packets that can be sent gets blocked by the RoboRIO, so all methods work identically in
 * all robot modes.
 *
 * <p>All methods are thread safe, however the CANData object passed into the read methods and the
 * byte[] passed into the write methods need to not be modified for the duration of their respective
 * calls.
 */
public class CAN implements Closeable {
  /** Team manufacturer. */
  public static final int kTeamManufacturer = CANAPITypes.CANManufacturer.kTeamUse.id;

  /** Team device type. */
  public static final int kTeamDeviceType = CANAPITypes.CANDeviceType.kMiscellaneous.id;

  private final int m_handle;

  /**
   * Create a new CAN communication interface with the specific device ID. This uses the team
   * manufacturer and device types. The device ID is 6 bits (0-63).
   *
   * @param busId The bus ID
   * @param deviceId The device id
   */
  public CAN(int busId, int deviceId) {
    this(busId, deviceId, kTeamManufacturer, kTeamDeviceType);
  }

  /**
   * Create a new CAN communication interface with a specific device ID, manufacturer and device
   * type. The device ID is 6 bits, the manufacturer is 8 bits, and the device type is 5 bits.
   *
   * @param busId The bus ID
   * @param deviceId The device ID
   * @param deviceManufacturer The device manufacturer
   * @param deviceType The device type
   */
  public CAN(int busId, int deviceId, int deviceManufacturer, int deviceType) {
    m_handle = CANAPIJNI.initializeCAN(busId, deviceManufacturer, deviceId, deviceType);
    HAL.reportUsage("CAN[" + deviceType + "][" + deviceManufacturer + "][" + deviceId + "]", "");
  }

  /** Closes the CAN communication. */
  @Override
  public void close() {
    if (m_handle != 0) {
      CANAPIJNI.cleanCAN(m_handle);
    }
  }

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   */
  public void writePacket(int apiId, byte[] data, int dataLength, int flags) {
    CANAPIJNI.writeCANPacket(m_handle, apiId, data, dataLength, flags);
  }

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is 10 bits. The RoboRIO
   * will automatically repeat the packet at the specified interval
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   * @param repeatMs The period to repeat the packet at.
   */
  public void writePacketRepeating(
      int apiId, byte[] data, int dataLength, int flags, int repeatMs) {
    CANAPIJNI.writeCANPacketRepeating(m_handle, apiId, data, dataLength, flags, repeatMs);
  }

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10 bits. The length by spec
   * must match what is returned by the responding device
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   */
  public void writeRTRFrame(int apiId, byte[] data, int dataLength, int flags) {
    CANAPIJNI.writeCANRTRFrame(m_handle, apiId, data, dataLength, flags);
  }

  /**
   * Write a packet to the CAN device with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   * @return TODO
   */
  public int writePacketNoThrow(int apiId, byte[] data, int dataLength, int flags) {
    return CANAPIJNI.writeCANPacketNoThrow(m_handle, apiId, data, dataLength, flags);
  }

  /**
   * Write a repeating packet to the CAN device with a specific ID. This ID is 10 bits. The RoboRIO
   * will automatically repeat the packet at the specified interval
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   * @param repeatMs The period to repeat the packet at.
   * @return TODO
   */
  public int writePacketRepeatingNoThrow(
      int apiId, byte[] data, int dataLength, int flags, int repeatMs) {
    return CANAPIJNI.writeCANPacketRepeatingNoThrow(
        m_handle, apiId, data, dataLength, flags, repeatMs);
  }

  /**
   * Write an RTR frame to the CAN device with a specific ID. This ID is 10 bits. The length by spec
   * must match what is returned by the responding device
   *
   * @param apiId The API ID to write.
   * @param data The data to write
   * @param dataLength The data length
   * @param flags The flags
   * @return TODO
   */
  public int writeRTRFrameNoThrow(int apiId, byte[] data, int dataLength, int flags) {
    return CANAPIJNI.writeCANRTRFrameNoThrow(m_handle, apiId, data, dataLength, flags);
  }

  /**
   * Stop a repeating packet with a specific ID. This ID is 10 bits.
   *
   * @param apiId The API ID to stop repeating
   */
  public void stopPacketRepeating(int apiId) {
    CANAPIJNI.stopCANPacketRepeating(m_handle, apiId);
  }

  /**
   * Read a new CAN packet. This will only return properly once per packet received. Multiple calls
   * without receiving another packet will return false.
   *
   * @param apiId The API ID to read.
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  public boolean readPacketNew(int apiId, CANReceiveMessage data) {
    return CANAPIJNI.readCANPacketNew(m_handle, apiId, data);
  }

  /**
   * Read a CAN packet. This will continuously return the last packet received, without accounting
   * for packet age.
   *
   * @param apiId The API ID to read.
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  public boolean readPacketLatest(int apiId, CANReceiveMessage data) {
    return CANAPIJNI.readCANPacketLatest(m_handle, apiId, data);
  }

  /**
   * Read a CAN packet. This will return the last packet received until the packet is older than the
   * requested timeout. Then it will return false.
   *
   * @param apiId The API ID to read.
   * @param timeoutMs The timeout time for the packet
   * @param data Storage for the received data.
   * @return True if the data is valid, otherwise false.
   */
  public boolean readPacketTimeout(int apiId, CANReceiveMessage data, int timeoutMs) {
    return CANAPIJNI.readCANPacketTimeout(m_handle, apiId, data, timeoutMs);
  }
}
