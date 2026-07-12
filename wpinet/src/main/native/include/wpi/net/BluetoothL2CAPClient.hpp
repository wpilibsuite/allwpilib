// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>

namespace wpi::net::uv {
class Loop;
}  // namespace wpi::net::uv

namespace wpi::net {

/**
 * Bluetooth LE address type.
 */
enum class BluetoothAddressType { kPublic, kRandom };

/**
 * Connection status for a Bluetooth LE L2CAP Credit-Based Mode client.
 */
struct BluetoothL2CAPConnectionStatus {
  bool supported = false;
  bool targetConfigured = false;
  bool connecting = false;
  bool connected = false;
  std::string targetAddress;
  BluetoothAddressType addressType = BluetoothAddressType::kRandom;
  std::string status;
  std::string error;
  uint64_t packetsReceived = 0;
  uint64_t packetsSent = 0;
};

/**
 * Connection configuration for a Bluetooth LE L2CAP Credit-Based Mode client.
 */
struct BluetoothL2CAPClientConfig {
  std::string address;
  BluetoothAddressType addressType = BluetoothAddressType::kRandom;
  uint16_t psm = 0;
  size_t maxPacketSize = 512;
};

/**
 * Packet-oriented Bluetooth LE L2CAP Credit-Based Mode client.
 *
 * Each Send() call writes one L2CAP SDU. Each packet callback receives one
 * packet from the platform transport when the platform exposes SDU boundaries.
 */
class BluetoothL2CAPClient {
 public:
  using PacketCallback = std::function<void(std::span<const uint8_t> packet)>;
  using StatusCallback =
      std::function<void(const BluetoothL2CAPConnectionStatus& status)>;

  /**
   * Creates a Bluetooth LE L2CAP Credit-Based Mode client.
   *
   * @param loop libuv loop used to marshal callbacks.
   * @param packetCallback callback invoked for received packets.
   * @param statusCallback callback invoked when connection status changes.
   * @return client instance, or nullptr if the loop callback channel could not
   *         be created.
   */
  static std::shared_ptr<BluetoothL2CAPClient> Create(
      wpi::net::uv::Loop& loop, PacketCallback packetCallback,
      StatusCallback statusCallback = {});

  /**
   * Destroys the client and closes any active connection.
   */
  ~BluetoothL2CAPClient();

  BluetoothL2CAPClient(const BluetoothL2CAPClient&) = delete;
  BluetoothL2CAPClient& operator=(const BluetoothL2CAPClient&) = delete;

  /**
   * Returns whether this platform has an implementation for the transport.
   *
   * @return true if the platform implementation is available.
   */
  static bool IsSupported();

  /**
   * Starts connecting to a Bluetooth LE L2CAP PSM.
   *
   * @param config connection configuration.
   * @return true if the request was accepted.
   */
  bool Connect(BluetoothL2CAPClientConfig config);

  /**
   * Disconnects the active connection, if any.
   *
   * @param reason status text to publish after disconnecting.
   */
  void Disconnect(std::string_view reason = "Disconnected");

  /**
   * Sends one packet as an L2CAP SDU.
   *
   * @param packet packet payload to send.
   * @return true if the packet was accepted for sending.
   */
  bool Send(std::span<const uint8_t> packet);

  /**
   * Gets the current connection status.
   *
   * @return current connection status.
   */
  BluetoothL2CAPConnectionStatus GetStatus() const;

 private:
  class Impl;

  explicit BluetoothL2CAPClient(std::shared_ptr<Impl> impl);

  std::shared_ptr<Impl> m_impl;
};

}  // namespace wpi::net
