// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRP.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "wpi/gui/wpigui.hpp"
#include "wpi/net/raw_uv_ostream.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace uv = wpi::net::uv;

using namespace wpilibxrp;

namespace {

constexpr uint16_t XRP_BLUETOOTH_PSM = 0x0081;
constexpr size_t MAX_BLUETOOTH_PACKET_SIZE = 512;
constexpr size_t MAX_LATENCY_SEND_TIMES = 512;
constexpr uint32_t INVALID_CONTROL_RX_AGE_US = UINT32_MAX;
constexpr const char* XRP_GATT_SERVICE_UUID =
    "7d2ea28a-f7bd-485d-9d6a-2c3f0b214a3f";
constexpr const char* XRP_GATT_CONTROL_CHARACTERISTIC_UUID =
    "7d2ea28b-f7bd-485d-9d6a-2c3f0b214a3f";
constexpr const char* XRP_GATT_STATUS_CHARACTERISTIC_UUID =
    "7d2ea28c-f7bd-485d-9d6a-2c3f0b214a3f";
constexpr const char* XRP_BLUETOOTH_SETTINGS_FILE = "WPILibXRPBluetooth.json";
constexpr const char* XRP_BLUETOOTH_SETTINGS_ADDRESS_KEY = "address";
constexpr const char* XRP_BLUETOOTH_SETTINGS_ADDRESS_TYPE_KEY = "addressType";
constexpr const char* XRP_BLUETOOTH_SETTINGS_NAME_KEY = "name";

struct SavedBluetoothTarget {
  std::string address;
  std::string name;
  XRPBluetoothAddressType addressType = XRPBluetoothAddressType::RANDOM;
};

struct TimingEcho {
  uint16_t lastControlSeq = 0;
  uint32_t controlRxAgeUs = 0;
};

const char* AddressTypeToString(XRPBluetoothAddressType type) {
  return type == XRPBluetoothAddressType::PUBLIC ? "public" : "random";
}

XRPBluetoothAddressType ParseAddressType(std::string_view type) {
  if (type == "public" || type == "PUBLIC" || type == "Public") {
    return XRPBluetoothAddressType::PUBLIC;
  }
  return XRPBluetoothAddressType::RANDOM;
}

std::string GetBluetoothSettingsPath() {
  std::string dir = wpi::gui::GetPlatformSaveFileDir();
  if (dir.empty()) {
    return {};
  }
  return dir + XRP_BLUETOOTH_SETTINGS_FILE;
}

std::optional<SavedBluetoothTarget> LoadBluetoothTarget() {
  std::string path = GetBluetoothSettingsPath();
  if (path.empty()) {
    return std::nullopt;
  }

  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(path);
  if (!fileBuffer) {
    return std::nullopt;
  }

  auto buffer = fileBuffer.value()->GetCharBuffer();
  auto parsed = wpi::util::json::parse({buffer.data(), buffer.size()});
  if (!parsed || !parsed->is_object()) {
    return std::nullopt;
  }

  const auto* address = parsed->lookup(XRP_BLUETOOTH_SETTINGS_ADDRESS_KEY);
  if (address == nullptr || !address->is_string() ||
      address->get_string().empty()) {
    return std::nullopt;
  }

  SavedBluetoothTarget target;
  target.address = address->get_string();

  const auto* name = parsed->lookup(XRP_BLUETOOTH_SETTINGS_NAME_KEY);
  if (name != nullptr && name->is_string()) {
    target.name = name->get_string();
  }

  const auto* addressType =
      parsed->lookup(XRP_BLUETOOTH_SETTINGS_ADDRESS_TYPE_KEY);
  if (addressType != nullptr && addressType->is_string()) {
    target.addressType = ParseAddressType(addressType->get_string());
  }

  return target;
}

void SaveBluetoothTarget(std::string_view address,
                         XRPBluetoothAddressType addressType,
                         std::string_view name) {
  if (address.empty()) {
    return;
  }

  std::string path = GetBluetoothSettingsPath();
  if (path.empty()) {
    return;
  }

  std::error_code ec;
  fs::create_directories(fs::path{path}.parent_path(), ec);
  if (ec) {
    wpi::util::print(stderr,
                     "Failed to create HALSim XRP Bluetooth settings "
                     "directory: {}\n",
                     ec.message());
    return;
  }

  wpi::util::raw_fd_ostream os{path, ec};
  if (ec) {
    wpi::util::print(stderr, "Failed to save HALSim XRP Bluetooth target: {}\n",
                     ec.message());
    return;
  }

  wpi::util::json settings;
  settings[XRP_BLUETOOTH_SETTINGS_ADDRESS_KEY] = std::string{address};
  settings[XRP_BLUETOOTH_SETTINGS_NAME_KEY] = std::string{name};
  settings[XRP_BLUETOOTH_SETTINGS_ADDRESS_TYPE_KEY] =
      AddressTypeToString(addressType);
  settings.marshal(os, true, 2);
  os << '\n';
}

uint16_t ReadUint16BE(std::span<const uint8_t> data) {
  return static_cast<uint16_t>(data[0] << 8) | data[1];
}

bool SkipField(std::span<const uint8_t>* packet, size_t size) {
  if (packet->size() < size) {
    return false;
  }
  *packet = packet->subspan(size);
  return true;
}

std::optional<TimingEcho> ReadTimingEcho(std::span<const uint8_t> packet) {
  if (packet.size() < PACKET_HEADER_SIZE) {
    return std::nullopt;
  }

  uint16_t fieldMask = ReadUint16BE(packet.subspan(3, 2));
  if ((fieldMask & ~STATUS_ALL_FIELDS) != 0 ||
      (fieldMask & STATUS_TIMING) == 0) {
    return std::nullopt;
  }

  packet = packet.subspan(PACKET_HEADER_SIZE);
  for (int encoder = 0; encoder < 4; encoder++) {
    if ((fieldMask & (STATUS_ENCODER_0 << encoder)) != 0 &&
        !SkipField(&packet, 8)) {
      return std::nullopt;
    }
  }

  if ((fieldMask & STATUS_DIO) != 0 && !SkipField(&packet, 2)) {
    return std::nullopt;
  }
  if ((fieldMask & STATUS_GYRO) != 0 && !SkipField(&packet, 24)) {
    return std::nullopt;
  }
  if ((fieldMask & STATUS_ACCEL) != 0 && !SkipField(&packet, 12)) {
    return std::nullopt;
  }
  for (int analog = 0; analog < 3; analog++) {
    if ((fieldMask & (STATUS_ANALOG_0 << analog)) != 0 &&
        !SkipField(&packet, 2)) {
      return std::nullopt;
    }
  }

  if (packet.size() < 4) {
    return std::nullopt;
  }

  uint16_t controlRxAge10Us = ReadUint16BE(packet.subspan(2, 2));
  uint32_t controlRxAgeUs = controlRxAge10Us == INVALID_CONTROL_RX_AGE_10_US
                                ? INVALID_CONTROL_RX_AGE_US
                                : controlRxAge10Us * CONTROL_RX_AGE_UNIT_US;

  return TimingEcho{ReadUint16BE(packet.subspan(0, 2)), controlRxAgeUs};
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
      [weakSelf](const wpi::net::BluetoothLEPacketConnectionStatus& status) {
        if (auto self = weakSelf.lock()) {
          if (!status.connected) {
            self->m_xrp.ResetStatusPacketSequence();
          }
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
  bool addressFromEnv = address != nullptr;
  if (address != nullptr) {
    m_targetAddress = address;
  }

  const char* addressType = std::getenv("HALSIMXRP_BT_ADDRESS_TYPE");
  bool addressTypeFromEnv = addressType != nullptr;
  if (addressType != nullptr) {
    m_targetAddressType = ParseAddressType(addressType);
  }

  if (!addressFromEnv) {
    if (auto savedTarget = LoadBluetoothTarget()) {
      m_targetAddress = std::move(savedTarget->address);
      m_targetName = std::move(savedTarget->name);
      if (!addressTypeFromEnv) {
        m_targetAddressType = savedTarget->addressType;
      }
    }
  }

  {
    std::scoped_lock lock(m_statusMutex);
    m_status = m_bluetoothClient->GetStatus();
    m_status.targetAddress = m_targetAddress;
    m_status.targetName = m_targetName;
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
    if (m_targetName.empty()) {
      wpi::util::println("HALSimXRP Bluetooth target: {} ({})", m_targetAddress,
                         AddressTypeToString(m_targetAddressType));
    } else {
      wpi::util::println("HALSimXRP Bluetooth target: {} ({}, {})",
                         m_targetName, m_targetAddress,
                         AddressTypeToString(m_targetAddressType));
    }
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
    ConnectBluetooth(m_targetAddress, m_targetAddressType, m_targetName);
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
                                 XRPBluetoothAddressType type,
                                 std::string name) {
  wpi::net::BluetoothLEPacketClientConfig config;
  std::string targetName;
  {
    std::scoped_lock lock(m_statusMutex);
    m_targetAddress = std::move(address);
    m_targetName = std::move(name);
    m_targetAddressType = type;
    m_status.targetAddress = m_targetAddress;
    m_status.targetName = m_targetName;
    m_status.addressType = m_targetAddressType;
    m_status.targetConfigured = !m_targetAddress.empty();
    m_status.error.clear();
    targetName = m_targetName;

    config.address = m_targetAddress;
    config.addressType = m_targetAddressType;
    config.psm = XRP_BLUETOOTH_PSM;
    config.gattServiceUuid = XRP_GATT_SERVICE_UUID;
    config.gattControlCharacteristicUuid = XRP_GATT_CONTROL_CHARACTERISTIC_UUID;
    config.gattStatusCharacteristicUuid = XRP_GATT_STATUS_CHARACTERISTIC_UUID;
    config.maxPacketSize = MAX_BLUETOOTH_PACKET_SIZE;
  }

  SaveBluetoothTarget(config.address, config.addressType, targetName);
  m_xrp.ResetStatusPacketSequence();

  if (m_bluetoothClient) {
    m_bluetoothClient->Connect(std::move(config));
  }
}

void HALSimXRP::RememberBluetoothTarget(std::string address,
                                        XRPBluetoothAddressType type,
                                        std::string name) {
  std::string targetAddress;
  std::string targetName;
  {
    std::scoped_lock lock(m_statusMutex);
    m_targetAddress = std::move(address);
    m_targetName = std::move(name);
    m_targetAddressType = type;
    m_status.targetAddress = m_targetAddress;
    m_status.targetName = m_targetName;
    m_status.addressType = m_targetAddressType;
    m_status.targetConfigured = !m_targetAddress.empty();
    targetAddress = m_targetAddress;
    targetName = m_targetName;
  }

  SaveBluetoothTarget(targetAddress, type, targetName);
}

void HALSimXRP::DisconnectBluetooth() {
  m_xrp.ResetStatusPacketSequence();
  if (m_bluetoothClient) {
    m_bluetoothClient->Disconnect("Disconnected by user");
  }
}

bool HALSimXRP::RenameBluetoothDevice(std::string_view deviceName) {
  if (!m_bluetoothClient || deviceName.empty() ||
      deviceName.size() > CONTROL_DEVICE_NAME_MAX_LENGTH) {
    return false;
  }

  wpi::util::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::net::raw_uv_ostream stream{sendBufs, [&] {
                                    std::lock_guard lock(m_buffer_mutex);
                                    return GetBufferPool().Allocate();
                                  }};
  m_xrp.SetupRenameDeviceBuffer(stream, deviceName);

  size_t packetSize = 0;
  for (const auto& buf : sendBufs) {
    packetSize += buf.len;
  }

  std::vector<uint8_t> packet;
  if (packetSize <= MAX_BLUETOOTH_PACKET_SIZE) {
    packet.reserve(packetSize);
    for (const auto& buf : sendBufs) {
      const auto* data = reinterpret_cast<const uint8_t*>(buf.base);
      packet.insert(packet.end(), data, data + buf.len);
    }
  }

  {
    std::lock_guard lock(m_buffer_mutex);
    GetBufferPool().Release(sendBufs);
  }

  return !packet.empty() && m_bluetoothClient->Send(packet);
}

XRPConnectionStatus HALSimXRP::GetConnectionStatus() const {
  std::scoped_lock lock(m_statusMutex);
  return m_status;
}

XRPDataSnapshot HALSimXRP::GetDataSnapshot() const {
  return m_xrp.GetDataSnapshot();
}

void HALSimXRP::ParsePacket(std::span<const uint8_t> packet) {
  if (packet.size() < 3) {
    return;
  }

  // Hand this off to the XRP object to deal with the messages
  m_xrp.HandleXRPUpdate(packet);
  UpdateLatencyFromXRP(packet);
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

      if (m_bluetoothClient->Send(packet)) {
        RecordControlPacketSent(packet);
      }
    } else {
      SetError("XRP packet is larger than Bluetooth transport MTU");
    }
  }

  std::lock_guard lock(m_buffer_mutex);
  GetBufferPool().Release(sendBufs);
}

void HALSimXRP::RecordControlPacketSent(std::span<const uint8_t> packet) {
  if (packet.size() < 2) {
    return;
  }

  uint16_t seq = ReadUint16BE(packet.subspan(0, 2));
  m_controlPacketSendTimes[seq] = std::chrono::steady_clock::now();
  m_controlPacketSendOrder.push_back(seq);
  while (m_controlPacketSendOrder.size() > MAX_LATENCY_SEND_TIMES) {
    m_controlPacketSendTimes.erase(m_controlPacketSendOrder.front());
    m_controlPacketSendOrder.pop_front();
  }
}

void HALSimXRP::UpdateLatencyFromXRP(std::span<const uint8_t> packet) {
  auto timingEcho = ReadTimingEcho(packet);
  if (!timingEcho || timingEcho->controlRxAgeUs == INVALID_CONTROL_RX_AGE_US) {
    return;
  }

  if (m_haveLastLatencyControlSeq &&
      timingEcho->lastControlSeq == m_lastLatencyControlSeq) {
    return;
  }

  auto sendTime = m_controlPacketSendTimes.find(timingEcho->lastControlSeq);
  if (sendTime == m_controlPacketSendTimes.end()) {
    return;
  }

  auto now = std::chrono::steady_clock::now();
  auto roundTripLatencyMs =
      std::chrono::duration<double, std::milli>(now - sendTime->second).count();

  m_haveLastLatencyControlSeq = true;
  m_lastLatencyControlSeq = timingEcho->lastControlSeq;

  std::scoped_lock lock(m_statusMutex);
  m_status.latencyAvailable = true;
  m_status.latencyControlSeq = timingEcho->lastControlSeq;
  m_status.roundTripLatencyMs = roundTripLatencyMs;
  m_status.xrpControlRxAgeMs = timingEcho->controlRxAgeUs / 1000.0;
}

void HALSimXRP::SetError(std::string_view error) {
  std::scoped_lock lock(m_statusMutex);
  m_status.error = error;
  m_status.status = error;
  m_status.connecting = false;
  m_status.connected = false;
}
