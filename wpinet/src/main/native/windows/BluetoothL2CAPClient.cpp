// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothL2CAPClient.hpp"

#include <memory>
#include <mutex>
#include <utility>

#include "wpi/net/uv/Loop.hpp"

using namespace wpi::net;

namespace {

constexpr std::string_view kUnsupportedMessage =
    "Bluetooth LE L2CAP Credit-Based Mode is not supported by the Windows "
    "desktop Bluetooth socket API";

}  // namespace

class BluetoothL2CAPClient::Impl {
 public:
  static std::shared_ptr<Impl> Create(wpi::net::uv::Loop&,
                                      PacketCallback packetCallback,
                                      StatusCallback statusCallback) {
    return std::make_shared<Impl>(std::move(packetCallback),
                                  std::move(statusCallback));
  }

  Impl(PacketCallback packetCallback, StatusCallback statusCallback)
      : m_packetCallback{std::move(packetCallback)},
        m_statusCallback{std::move(statusCallback)} {
    m_status.supported = false;
    m_status.status = kUnsupportedMessage;
    m_status.error = kUnsupportedMessage;
  }

  bool Connect(BluetoothL2CAPClientConfig config) {
    UpdateStatus([&](auto& status) {
      status.targetAddress = std::move(config.address);
      status.addressType = config.addressType;
      status.targetConfigured = !status.targetAddress.empty();
      status.connecting = false;
      status.connected = false;
      status.status = kUnsupportedMessage;
      status.error = kUnsupportedMessage;
    });
    return false;
  }

  void Disconnect(std::string_view reason) {
    UpdateStatus([&](auto& status) {
      status.connecting = false;
      status.connected = false;
      status.status = reason;
    });
  }

  bool Send(std::span<const uint8_t>) { return false; }

  BluetoothL2CAPConnectionStatus GetStatus() const {
    std::scoped_lock lock{m_statusMutex};
    return m_status;
  }

 private:
  template <typename F>
  void UpdateStatus(F&& func) {
    BluetoothL2CAPConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      func(m_status);
      snapshot = m_status;
    }
    if (m_statusCallback) {
      m_statusCallback(snapshot);
    }
  }

  [[maybe_unused]]
  PacketCallback m_packetCallback;
  StatusCallback m_statusCallback;

  mutable std::mutex m_statusMutex;
  BluetoothL2CAPConnectionStatus m_status;
};

std::shared_ptr<BluetoothL2CAPClient> BluetoothL2CAPClient::Create(
    wpi::net::uv::Loop& loop, PacketCallback packetCallback,
    StatusCallback statusCallback) {
  auto impl =
      Impl::Create(loop, std::move(packetCallback), std::move(statusCallback));
  if (!impl) {
    return nullptr;
  }
  return std::shared_ptr<BluetoothL2CAPClient>(
      new BluetoothL2CAPClient{std::move(impl)});
}

BluetoothL2CAPClient::BluetoothL2CAPClient(std::shared_ptr<Impl> impl)
    : m_impl{std::move(impl)} {}

BluetoothL2CAPClient::~BluetoothL2CAPClient() = default;

bool BluetoothL2CAPClient::IsSupported() {
  return false;
}

bool BluetoothL2CAPClient::Connect(BluetoothL2CAPClientConfig config) {
  return m_impl->Connect(std::move(config));
}

void BluetoothL2CAPClient::Disconnect(std::string_view reason) {
  m_impl->Disconnect(reason);
}

bool BluetoothL2CAPClient::Send(std::span<const uint8_t> packet) {
  return m_impl->Send(packet);
}

BluetoothL2CAPConnectionStatus BluetoothL2CAPClient::GetStatus() const {
  return m_impl->GetStatus();
}
