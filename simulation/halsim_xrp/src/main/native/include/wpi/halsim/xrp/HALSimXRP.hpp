// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <string_view>

#include "wpi/halsim/ws_core/HALSimBaseWebSocketConnection.hpp"
#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/halsim/xrp/XRP.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Buffer.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Poll.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpilibxrp {

enum class XRPBluetoothAddressType { kPublic, kRandom };

struct XRPConnectionStatus {
  bool bluetoothSupported = false;
  bool targetConfigured = false;
  bool connecting = false;
  bool connected = false;
  std::string targetAddress;
  XRPBluetoothAddressType addressType = XRPBluetoothAddressType::kRandom;
  std::string status;
  std::string error;
  uint64_t packetsReceived = 0;
  uint64_t packetsSent = 0;
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
  void ConnectBluetooth(std::string address, XRPBluetoothAddressType type);
  void DisconnectBluetooth();
  XRPConnectionStatus GetConnectionStatus() const;

  void ParsePacket(std::span<const uint8_t> packet);
  void OnNetValueChanged(const wpi::util::json& msg);
  void OnSimValueChanged(const wpi::util::json& simData) override;

  const std::string& GetTargetAddress() const { return m_targetAddress; }
  XRPBluetoothAddressType GetTargetAddressType() const {
    return m_targetAddressType;
  }
  wpi::net::uv::Loop& GetLoop() { return m_loop; }

  UvExecFunc& GetExec() { return *m_exec; }

 private:
  XRP m_xrp;

  wpi::net::uv::Loop& m_loop;
  std::shared_ptr<wpi::net::uv::Poll> m_bluetoothPoll;
  std::shared_ptr<UvExecFunc> m_exec;

  wpilibws::ProviderContainer& m_providers;
  wpilibws::HALSimWSProviderSimDevices& m_simDevicesProvider;

  mutable std::mutex m_statusMutex;
  XRPConnectionStatus m_status;

  std::string m_targetAddress;
  XRPBluetoothAddressType m_targetAddressType =
      XRPBluetoothAddressType::kRandom;

  int m_bluetoothSocket = -1;
  bool m_providersConnected = false;

  void SendStateToXRP();
  void ConnectBluetoothOnLoop();
  void CloseBluetoothOnLoop(std::string_view reason);
  void CheckBluetoothConnect();
  void ReadBluetoothPackets();
  void SendPacketToXRP(std::span<wpi::net::uv::Buffer> sendBufs);
  void SetStatus(std::string_view status);
  void SetError(std::string_view error);
  void RegisterSimProviders();
  wpi::net::uv::SimpleBufferPool<4>& GetBufferPool();
  std::mutex m_buffer_mutex;
};

}  // namespace wpilibxrp
