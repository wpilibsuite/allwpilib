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
#include "wpi/net/BluetoothL2CAPClient.hpp"
#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Buffer.hpp"
#include "wpi/net/uv/Loop.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpilibxrp {

using XRPBluetoothAddressType = wpi::net::BluetoothAddressType;
using XRPConnectionStatus = wpi::net::BluetoothL2CAPConnectionStatus;

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
  std::shared_ptr<UvExecFunc> m_exec;
  std::shared_ptr<wpi::net::BluetoothL2CAPClient> m_bluetoothClient;

  wpilibws::ProviderContainer& m_providers;
  wpilibws::HALSimWSProviderSimDevices& m_simDevicesProvider;

  mutable std::mutex m_statusMutex;
  XRPConnectionStatus m_status;

  std::string m_targetAddress;
  XRPBluetoothAddressType m_targetAddressType =
      XRPBluetoothAddressType::kRandom;

  bool m_providersConnected = false;

  void SendStateToXRP();
  void SendPacketToXRP(std::span<wpi::net::uv::Buffer> sendBufs);
  void SetError(std::string_view error);
  void RegisterSimProviders();
  wpi::net::uv::SimpleBufferPool<4>& GetBufferPool();
  std::mutex m_buffer_mutex;
};

}  // namespace wpilibxrp
