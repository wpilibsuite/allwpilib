// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * CAN API HAL JNI Functions.
 *
 * @see "hal/CANAPI.h"
 */
public class CANAPIJNI extends JNIWrapper {
  /**
   * Reads the current value of the millisecond-resolution timer that the CAN API functions use as a
   * time base.
   *
   * @return Current value of timer used as a base time by the CAN API in milliseconds.
   * @see "HAL_GetCANPacketBaseTime"
   */
  public static native long getCANPacketBaseTime();

  /**
   * Initializes a CAN device.
   *
   * <p>These follow the FIRST standard CAN layout.
   * https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
   *
   * @param manufacturer the can manufacturer
   * @param deviceId the device ID (0-63)
   * @param deviceType the device type
   * @return the created CAN handle
   * @see "HAL_InitializeCAN"
   */
  public static native int initializeCAN(int manufacturer, int deviceId, int deviceType);

  /**
   * Frees a CAN device.
   *
   * @param handle the CAN handle
   * @see "HAL_CleanCAN"
   */
  public static native void cleanCAN(int handle);

  /**
   * Writes a packet to the CAN device with a specific ID.
   *
   * <p>This ID is 10 bits.
   *
   * @param handle the CAN handle
   * @param data the data to write (0-8 bytes)
   * @param apiId the ID to write (0-1023 bits)
   * @see "HAL_WriteCANPacket"
   */
  public static native void writeCANPacket(int handle, byte[] data, int apiId);

  /**
   * Writes a repeating packet to the CAN device with a specific ID.
   *
   * <p>This ID is 10 bits.
   *
   * <p>The RoboRIO will automatically repeat the packet at the specified interval
   *
   * @param handle the CAN handle
   * @param data the data to write (0-8 bytes)
   * @param apiId the ID to write (0-1023)
   * @param repeatMs the period to repeat in ms
   * @see "HAL_WriteCANPacketRepeating"
   */
  public static native void writeCANPacketRepeating(
      int handle, byte[] data, int apiId, int repeatMs);

  /**
   * Writes an RTR frame of the specified length to the CAN device with the specific ID.
   *
   * <p>By spec, the length must be equal to the length sent by the other device, otherwise behavior
   * is unspecified.
   *
   * @param handle the CAN handle
   * @param length the length of data to request (0-8)
   * @param apiId the ID to write (0-1023)
   * @see "HAL_WriteCANRTRFrame"
   */
  public static native void writeCANRTRFrame(int handle, int length, int apiId);

  /**
   * Writes a packet to the CAN device with a specific ID without throwing on error.
   *
   * <p>This ID is 10 bits.
   *
   * @param handle the CAN handle
   * @param data the data to write (0-8 bytes)
   * @param apiId the ID to write (0-1023 bits)
   * @return Error status variable. 0 on success.
   * @see "HAL_WriteCANPacket"
   */
  public static native int writeCANPacketNoThrow(int handle, byte[] data, int apiId);

  /**
   * Writes a repeating packet to the CAN device with a specific ID without throwing on error.
   *
   * <p>This ID is 10 bits.
   *
   * <p>The RoboRIO will automatically repeat the packet at the specified interval
   *
   * @param handle the CAN handle
   * @param data the data to write (0-8 bytes)
   * @param apiId the ID to write (0-1023)
   * @param repeatMs the period to repeat in ms
   * @return Error status variable. 0 on success.
   * @see "HAL_WriteCANPacketRepeating"
   */
  public static native int writeCANPacketRepeatingNoThrow(
      int handle, byte[] data, int apiId, int repeatMs);

  /**
   * Writes an RTR frame of the specified length to the CAN device with the specific ID without
   * throwing on error.
   *
   * <p>By spec, the length must be equal to the length sent by the other device, otherwise behavior
   * is unspecified.
   *
   * @param handle the CAN handle
   * @param length the length of data to request (0-8)
   * @param apiId the ID to write (0-1023)
   * @return Error status variable. 0 on success.
   * @see "HAL_WriteCANRTRFrame"
   */
  public static native int writeCANRTRFrameNoThrow(int handle, int length, int apiId);

  /**
   * Stops a repeating packet with a specific ID.
   *
   * <p>This ID is 10 bits.
   *
   * @param handle the CAN handle
   * @param apiId the ID to stop repeating (0-1023)
   * @see "HAL_StopCANPacketRepeating"
   */
  public static native void stopCANPacketRepeating(int handle, int apiId);

  /**
   * Reads a new CAN packet.
   *
   * <p>This will only return properly once per packet received. Multiple calls without receiving
   * another packet will return false.
   *
   * @param handle the CAN handle
   * @param apiId the ID to read (0-1023)
   * @param data the packet data (8 bytes)
   * @return true on success, false on error
   * @see "HAL_ReadCANPacketNew"
   */
  public static native boolean readCANPacketNew(int handle, int apiId, CANData data);

  /**
   * Reads a CAN packet. The will continuously return the last packet received, without accounting
   * for packet age.
   *
   * @param handle the CAN handle
   * @param apiId the ID to read (0-1023)
   * @param data the packet data (8 bytes)
   * @return true on success, false on error
   * @see "HAL_ReadCANPacketLatest"
   */
  public static native boolean readCANPacketLatest(int handle, int apiId, CANData data);

  /**
   * Reads a CAN packet. The will return the last packet received until the packet is older then the
   * requested timeout. Then it will return false.
   *
   * @param handle the CAN handle
   * @param apiId the ID to read (0-1023)
   * @param timeoutMs the timeout time for the packet
   * @param data the packet data (8 bytes)
   * @return true on success, false on error
   * @see "HAL_ReadCANPacketTimeout"
   */
  public static native boolean readCANPacketTimeout(
      int handle, int apiId, int timeoutMs, CANData data);

  /** Utility class. */
  private CANAPIJNI() {}
}
