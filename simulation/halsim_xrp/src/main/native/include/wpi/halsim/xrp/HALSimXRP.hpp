// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

#include "wpi/halsim/ws_core/HALSimBaseWebSocketConnection.hpp"
#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/halsim/xrp/XRP.hpp"
#include "wpi/net/BluetoothLEPacketClient.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Buffer.hpp"
#include "wpi/net/uv/Loop.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpilibxrp {

using XRPBluetoothAddressType = wpi::net::BluetoothAddressType;

struct XRPConnectionStatus
    : public wpi::net::BluetoothLEPacketConnectionStatus {
  using Base = wpi::net::BluetoothLEPacketConnectionStatus;

  XRPConnectionStatus() = default;

  XRPConnectionStatus& operator=(const Base& status) {
    static_cast<Base&>(*this) = status;
    if (!connected) {
      latencyAvailable = false;
    }
    return *this;
  }

  bool latencyAvailable = false;
  std::string targetName;
  uint16_t latencyControlSeq = 0;
  double roundTripLatencyMs = 0.0;
  double xrpControlRxAgeMs = 0.0;
};

// This masquerades as a "WebSocket" so that we can reuse the
// stuff in halsim_ws_core
class HALSimXRP : public wpilibws::HALSimBaseWebSocketConnection,
                  public std::enable_shared_from_this<HALSimXRP> {
 public:
  using LoopFunc = std::function<void()>;
  using UvExecFunc = wpi::net::uv::Async<LoopFunc>;

  HALSimXRP(wpi::net::uv::Loop& loop, wpilibws::ProviderContainer& providers,
            wpilibws::HALSimWSProviderSimDevices& simDevicesProvider);
  ~HALSimXRP() override;
  HALSimXRP(const HALSimXRP&) = delete;
  HALSimXRP& operator=(const HALSimXRP&) = delete;

  bool Initialize();
  void Start();
  void ConnectBluetooth(std::string address, XRPBluetoothAddressType type,
                        std::string name = {});
  void DisconnectBluetooth();
  XRPConnectionStatus GetConnectionStatus() const;

  /**
   * Gets a snapshot of the latest XRP control and status data.
   *
   * @return Current XRP control and status data.
   */
  XRPDataSnapshot GetDataSnapshot() const;

  /**
   * Remembers the Bluetooth target without starting a connection.
   *
   * @param address platform-specific Bluetooth target address.
   * @param type Bluetooth address type.
   * @param name Bluetooth device display name.
   */
  void RememberBluetoothTarget(std::string address,
                               XRPBluetoothAddressType type,
                               std::string name = {});

  void ParsePacket(std::span<const uint8_t> packet);
  void OnNetValueChanged(const wpi::util::json& msg);
  void OnSimValueChanged(const wpi::util::json& simData) override;

  const std::string& GetTargetAddress() const { return m_targetAddress; }
  XRPBluetoothAddressType GetTargetAddressType() const {
    return m_targetAddressType;
  }
  const std::string& GetTargetName() const { return m_targetName; }
  wpi::net::uv::Loop& GetLoop() { return m_loop; }

  UvExecFunc& GetExec() { return *m_exec; }

 private:
  XRP m_xrp;

  wpi::net::uv::Loop& m_loop;
  std::shared_ptr<UvExecFunc> m_exec;
  std::shared_ptr<wpi::net::BluetoothLEPacketClient> m_bluetoothClient;

  wpilibws::ProviderContainer& m_providers;
  wpilibws::HALSimWSProviderSimDevices& m_simDevicesProvider;

  mutable std::mutex m_statusMutex;
  XRPConnectionStatus m_status;

  std::string m_targetAddress;
  std::string m_targetName;
  XRPBluetoothAddressType m_targetAddressType = XRPBluetoothAddressType::RANDOM;

  bool m_providersConnected = false;

  void RecordControlPacketSent(std::span<const uint8_t> packet);
  void UpdateLatencyFromXRP(std::span<const uint8_t> packet);
  void SendStateToXRP();
  void SendPacketToXRP(std::span<wpi::net::uv::Buffer> sendBufs);
  void SetError(std::string_view error);
  void RegisterSimProviders();
  wpi::net::uv::SimpleBufferPool<4>& GetBufferPool();
  std::mutex m_buffer_mutex;
  std::unordered_map<uint16_t, std::chrono::steady_clock::time_point>
      m_controlPacketSendTimes;
  std::deque<uint16_t> m_controlPacketSendOrder;
  uint16_t m_lastLatencyControlSeq = 0;
  bool m_haveLastLatencyControlSeq = false;
};

}  // namespace wpilibxrp
