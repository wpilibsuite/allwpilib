// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace wpi::net::uv {
class Loop;
}  // namespace wpi::net::uv

namespace wpi::net {

/**
 * Bluetooth LE address type.
 */
enum class BluetoothAddressType { PUBLIC, RANDOM };

/**
 * Active Bluetooth packet transport.
 */
enum class BluetoothPacketTransport { NONE, L2CAP, GATT };

/**
 * Discovered Bluetooth LE device.
 */
struct BluetoothLEDeviceInfo {
  std::string target;
  std::string name;
  BluetoothAddressType addressType = BluetoothAddressType::RANDOM;
  bool paired = false;
  bool pairable = false;
};

/**
 * Bluetooth LE device discovery result.
 */
struct BluetoothLEDeviceScanResult {
  bool supported = false;
  std::string status;
  std::string error;
  std::vector<BluetoothLEDeviceInfo> devices;
};

/**
 * Bluetooth LE pairing result.
 */
struct BluetoothLEPairingResult {
  bool supported = false;
  bool paired = false;
  std::string status;
  std::string error;
};

/**
 * Connection status for a Bluetooth LE packet client.
 */
struct BluetoothLEPacketConnectionStatus {
  bool supported = false;
  bool targetConfigured = false;
  bool connecting = false;
  bool connected = false;
  BluetoothPacketTransport transport = BluetoothPacketTransport::NONE;
  std::string targetAddress;
  BluetoothAddressType addressType = BluetoothAddressType::RANDOM;
  std::string status;
  std::string error;
  uint64_t packetsReceived = 0;
  uint64_t packetsSent = 0;
};

/**
 * Connection configuration for a Bluetooth LE packet client.
 */
struct BluetoothLEPacketClientConfig {
  std::string address;
  BluetoothAddressType addressType = BluetoothAddressType::RANDOM;
  uint16_t psm = 0;
  std::string gattServiceUuid;
  std::string gattControlCharacteristicUuid;
  std::string gattStatusCharacteristicUuid;
  size_t maxPacketSize = 512;
  bool preferL2CAP = true;
};

/**
 * Packet-oriented Bluetooth LE client.
 *
 * Each Send() call writes one packet using the best available platform
 * transport. Each packet callback receives one packet from a GATT notification
 * value or L2CAP SDU.
 */
class BluetoothLEPacketClient {
 public:
  using PacketCallback = std::function<void(std::span<const uint8_t> packet)>;
  using StatusCallback =
      std::function<void(const BluetoothLEPacketConnectionStatus& status)>;

  /**
   * Creates a Bluetooth LE packet client.
   *
   * @param loop libuv loop used to marshal callbacks.
   * @param packetCallback callback invoked for received packets.
   * @param statusCallback callback invoked when connection status changes.
   * @return client instance, or nullptr if the loop callback channel could not
   *         be created.
   */
  static std::shared_ptr<BluetoothLEPacketClient> Create(
      wpi::net::uv::Loop& loop, PacketCallback packetCallback,
      StatusCallback statusCallback = {});

  /**
   * Destroys the client and closes any active connection.
   */
  ~BluetoothLEPacketClient();

  BluetoothLEPacketClient(const BluetoothLEPacketClient&) = delete;
  BluetoothLEPacketClient& operator=(const BluetoothLEPacketClient&) = delete;

  /**
   * Returns whether this platform implementation supports explicit pairing.
   *
   * @return true if PairDevice() can pair devices on this platform.
   */
  static bool IsPairingSupported();

  /**
   * Scans for Bluetooth LE devices.
   *
   * @param timeout active scan duration. A zero duration returns cached devices
   *        on platforms that support cached discovery.
   * @return scan result, including discovered connection targets.
   */
  static BluetoothLEDeviceScanResult ScanDevices(
      std::chrono::milliseconds timeout);

  /**
   * Attempts to pair with a Bluetooth LE device.
   *
   * @param target platform-specific device target from ScanDevices().
   * @return pairing result.
   */
  static BluetoothLEPairingResult PairDevice(std::string_view target);

  /**
   * Starts connecting to a Bluetooth LE packet transport.
   *
   * @param config connection configuration.
   * @return true if the request was accepted.
   */
  bool Connect(BluetoothLEPacketClientConfig config);

  /**
   * Disconnects the active connection, if any.
   *
   * @param reason status text to publish after disconnecting.
   */
  void Disconnect(std::string_view reason = "Disconnected");

  /**
   * Sends one packet.
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
  BluetoothLEPacketConnectionStatus GetStatus() const;

 private:
  class Impl;

  explicit BluetoothLEPacketClient(std::shared_ptr<Impl> impl);

  std::shared_ptr<Impl> m_impl;
};

}  // namespace wpi::net
