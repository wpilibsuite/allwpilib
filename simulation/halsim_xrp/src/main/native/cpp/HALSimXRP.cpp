// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRP.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/net/raw_uv_ostream.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/print.hpp"

namespace uv = wpi::net::uv;

using namespace wpilibxrp;

namespace {

constexpr uint16_t XRP_BLUETOOTH_PSM = 0x0081;
constexpr size_t MAX_BLUETOOTH_PACKET_SIZE = 512;
constexpr const char* XRP_GATT_SERVICE_UUID =
    "7d2ea28a-f7bd-485d-9d6a-2c3f0b214a3f";
constexpr const char* XRP_GATT_CONTROL_CHARACTERISTIC_UUID =
    "7d2ea28b-f7bd-485d-9d6a-2c3f0b214a3f";
constexpr const char* XRP_GATT_STATUS_CHARACTERISTIC_UUID =
    "7d2ea28c-f7bd-485d-9d6a-2c3f0b214a3f";

const char* AddressTypeToString(XRPBluetoothAddressType type) {
  return type == XRPBluetoothAddressType::PUBLIC ? "public" : "random";
}

XRPBluetoothAddressType ParseAddressType(std::string_view type) {
  if (type == "public" || type == "PUBLIC" || type == "Public") {
    return XRPBluetoothAddressType::PUBLIC;
  }
  return XRPBluetoothAddressType::RANDOM;
}

}  // namespace

HALSimXRP::HALSimXRP(wpi::net::uv::Loop& loop,
                     wpilibws::ProviderContainer& providers,
                     wpilibws::HALSimWSProviderSimDevices& simDevicesProvider)
    : m_loop(loop),
      m_providers(providers),
      m_simDevicesProvider(simDevicesProvider) {
  m_loop.error.connect([](uv::Error err) {
    wpi::util::print(stderr, "HALSim XRP Client libuv Error: {}\n", err.str());
  });

  m_exec = UvExecFunc::Create(m_loop);
  if (m_exec) {
    m_exec->wakeup.connect([](auto func) { func(); });
  }
}

HALSimXRP::~HALSimXRP() = default;

bool HALSimXRP::Initialize() {
  if (!m_exec) {
    return false;
  }

  auto weakSelf = weak_from_this();
  m_bluetoothClient = wpi::net::BluetoothLEPacketClient::Create(
      m_loop,
      [weakSelf](std::span<const uint8_t> packet) {
        if (auto self = weakSelf.lock()) {
          self->ParsePacket(packet);
        }
      },
      [weakSelf](const XRPConnectionStatus& status) {
        if (auto self = weakSelf.lock()) {
          std::scoped_lock lock(self->m_statusMutex);
          self->m_status = status;
        }
      });
  if (!m_bluetoothClient) {
    return false;
  }

  const char* address = std::getenv("HALSIMXRP_BT_ADDRESS");
  if (address == nullptr) {
    address = std::getenv("HALSIMXRP_BLUETOOTH_ADDRESS");
  }
  if (address != nullptr) {
    m_targetAddress = address;
  }

  const char* addressType = std::getenv("HALSIMXRP_BT_ADDRESS_TYPE");
  if (addressType != nullptr) {
    m_targetAddressType = ParseAddressType(addressType);
  }

  {
    std::scoped_lock lock(m_statusMutex);
    m_status = m_bluetoothClient->GetStatus();
    m_status.targetAddress = m_targetAddress;
    m_status.targetConfigured = !m_targetAddress.empty();
    m_status.addressType = m_targetAddressType;
    if (m_status.supported) {
      m_status.status = m_status.targetConfigured
                            ? "Ready to connect"
                            : "Waiting for Bluetooth address";
    } else if (m_status.status.empty()) {
      m_status.status = "Bluetooth transport is unsupported on this platform";
    }
  }

  wpilibxrp::WPILibUpdateFunc func = [&](const wpi::util::json& data) {
    OnNetValueChanged(data);
  };

  m_xrp.SetWPILibUpdateFunc(func);

  wpi::util::println(
      "HALSimXRP Bluetooth transport: LE L2CAP Credit-Based Mode PSM 0x{:04x} "
      "with GATT fallback",
      XRP_BLUETOOTH_PSM);
  if (!m_targetAddress.empty()) {
    wpi::util::println("HALSimXRP Bluetooth target: {} ({})", m_targetAddress,
                       AddressTypeToString(m_targetAddressType));
  } else {
    wpi::util::println(
        "Set HALSIMXRP_BT_ADDRESS or use the XRP pairing window in the "
        "simulator GUI.");
  }

  return true;
}

void HALSimXRP::Start() {
  RegisterSimProviders();

  if (!m_targetAddress.empty() && m_bluetoothClient &&
      m_bluetoothClient->GetStatus().supported) {
    ConnectBluetooth(m_targetAddress, m_targetAddressType);
  }

  std::puts("HALSimXRP Initialized");
}

void HALSimXRP::RegisterSimProviders() {
  if (m_providersConnected) {
    return;
  }

  auto hws = shared_from_this();
  m_simDevicesProvider.OnNetworkConnected(hws);
  m_providers.ForEach(
      [hws](std::shared_ptr<wpilibws::HALSimWSBaseProvider> provider) {
        provider->OnNetworkConnected(hws);
      });
  m_providersConnected = true;
}

void HALSimXRP::ConnectBluetooth(std::string address,
                                 XRPBluetoothAddressType type) {
  wpi::net::BluetoothLEPacketClientConfig config;
  {
    std::scoped_lock lock(m_statusMutex);
    m_targetAddress = std::move(address);
    m_targetAddressType = type;
    m_status.targetAddress = m_targetAddress;
    m_status.addressType = m_targetAddressType;
    m_status.targetConfigured = !m_targetAddress.empty();
    m_status.error.clear();

    config.address = m_targetAddress;
    config.addressType = m_targetAddressType;
    config.psm = XRP_BLUETOOTH_PSM;
    config.gattServiceUuid = XRP_GATT_SERVICE_UUID;
    config.gattControlCharacteristicUuid = XRP_GATT_CONTROL_CHARACTERISTIC_UUID;
    config.gattStatusCharacteristicUuid = XRP_GATT_STATUS_CHARACTERISTIC_UUID;
    config.maxPacketSize = MAX_BLUETOOTH_PACKET_SIZE;
  }

  if (m_bluetoothClient) {
    m_bluetoothClient->Connect(std::move(config));
  }
}

void HALSimXRP::DisconnectBluetooth() {
  if (m_bluetoothClient) {
    m_bluetoothClient->Disconnect("Disconnected by user");
  }
}

XRPConnectionStatus HALSimXRP::GetConnectionStatus() const {
  std::scoped_lock lock(m_statusMutex);
  return m_status;
}

void HALSimXRP::ParsePacket(std::span<const uint8_t> packet) {
  if (packet.size() < 3) {
    return;
  }

  // Hand this off to the XRP object to deal with the messages
  m_xrp.HandleXRPUpdate(packet);
}

void HALSimXRP::OnNetValueChanged(const wpi::util::json& msg) {
  try {
    auto& type = msg.at("type").get_string();
    auto& device = msg.at("device").get_string();

    wpi::util::SmallString<64> key;
    key.append(type);
    if (!device.empty()) {
      key.append("/");
      key.append(device);
    }

    auto provider = m_providers.Get(key.str());
    if (provider) {
      provider->OnNetValueChanged(msg.at("data"));
    }
  } catch (std::logic_error& e) {
    wpi::util::print(stderr, "Error with incoming message: {}\n", e.what());
  }
}

void HALSimXRP::OnSimValueChanged(const wpi::util::json& simData) {
  // We'll use a signal from robot code to send all the data
  auto type = simData.lookup("type");
  if (type->is_string() && type->get_string() == "HAL") {
    auto halData = simData.lookup("data");
    if (halData && halData->contains(">sim_periodic_after")) {
      SendStateToXRP();
    }
  } else {
    m_xrp.HandleWPILibUpdate(simData);
  }
}

uv::SimpleBufferPool<4>& HALSimXRP::GetBufferPool() {
  static uv::SimpleBufferPool<4> bufferPool(MAX_BLUETOOTH_PACKET_SIZE);
  return bufferPool;
}

void HALSimXRP::SendStateToXRP() {
  wpi::util::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::net::raw_uv_ostream stream{sendBufs, [&] {
                                    std::lock_guard lock(m_buffer_mutex);
                                    return GetBufferPool().Allocate();
                                  }};
  m_xrp.SetupXRPSendBuffer(stream);

  auto self = shared_from_this();
  m_exec->Send([this, self, sendBufs]() mutable {
    SendPacketToXRP(std::span<uv::Buffer>{sendBufs.data(), sendBufs.size()});
  });
}

void HALSimXRP::SendPacketToXRP(std::span<uv::Buffer> sendBufs) {
  if (m_bluetoothClient) {
    size_t packetSize = 0;
    for (const auto& buf : sendBufs) {
      packetSize += buf.len;
    }

    if (packetSize <= MAX_BLUETOOTH_PACKET_SIZE) {
      std::vector<uint8_t> packet;
      packet.reserve(packetSize);
      for (const auto& buf : sendBufs) {
        const auto* data = reinterpret_cast<const uint8_t*>(buf.base);
        packet.insert(packet.end(), data, data + buf.len);
      }

      m_bluetoothClient->Send(packet);
    } else {
      SetError("XRP packet is larger than Bluetooth transport MTU");
    }
  }

  std::lock_guard lock(m_buffer_mutex);
  GetBufferPool().Release(sendBufs);
}

void HALSimXRP::SetError(std::string_view error) {
  std::scoped_lock lock(m_statusMutex);
  m_status.error = error;
  m_status.status = error;
  m_status.connecting = false;
  m_status.connected = false;
}
