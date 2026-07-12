// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#include "wpi/net/BluetoothL2CAPClient.hpp"

#include <Windows.h>

#include <atomic>
#include <cctype>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <combaseapi.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"

#pragma comment(lib, "windowsapp")

namespace uv = wpi::net::uv;
namespace bt = winrt::Windows::Devices::Bluetooth;
namespace gatt = winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
namespace streams = winrt::Windows::Storage::Streams;

using namespace wpi::net;

namespace {

uint8_t HexDigit(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  return 0xff;
}

bool ParseBluetoothAddress(std::string_view address, uint64_t* out) {
  if (address.size() != 17) {
    return false;
  }

  uint64_t parsed = 0;
  for (int i = 0; i < 6; ++i) {
    size_t pos = static_cast<size_t>(i * 3);
    if (i < 5 && address[pos + 2] != ':') {
      return false;
    }

    uint8_t high = HexDigit(address[pos]);
    uint8_t low = HexDigit(address[pos + 1]);
    if (high == 0xff || low == 0xff) {
      return false;
    }

    parsed = (parsed << 8) | static_cast<uint64_t>((high << 4) | low);
  }

  *out = parsed;
  return true;
}

winrt::guid ParseUuid(std::string_view uuid) {
  std::wstring wide;
  wide.reserve(uuid.size() + 2);
  wide.push_back(L'{');
  for (char ch : uuid) {
    wide.push_back(static_cast<wchar_t>(ch));
  }
  wide.push_back(L'}');

  GUID parsed{};
  if (FAILED(::CLSIDFromString(wide.c_str(), &parsed))) {
    return {};
  }
  return parsed;
}

std::string ToString(winrt::hresult_error const& error) {
  std::string result = winrt::to_string(error.message());
  if (result.empty()) {
    result = "HRESULT 0x" + std::to_string(static_cast<uint32_t>(error.code()));
  }
  return result;
}

streams::IBuffer ToBuffer(std::span<const uint8_t> packet) {
  streams::DataWriter writer;
  writer.WriteBytes(winrt::array_view<const uint8_t>{
      packet.data(), packet.data() + packet.size()});
  return writer.DetachBuffer();
}

std::vector<uint8_t> FromBuffer(streams::IBuffer const& buffer) {
  streams::DataReader reader = streams::DataReader::FromBuffer(buffer);
  std::vector<uint8_t> data(reader.UnconsumedBufferLength());
  if (!data.empty()) {
    reader.ReadBytes(
        winrt::array_view<uint8_t>{data.data(), data.data() + data.size()});
  }
  return data;
}

}  // namespace

class BluetoothL2CAPClient::Impl
    : public std::enable_shared_from_this<BluetoothL2CAPClient::Impl> {
 public:
  using LoopFunc = std::function<void()>;
  using UvExecFunc = uv::Async<LoopFunc>;

  static std::shared_ptr<Impl> Create(uv::Loop& loop,
                                      PacketCallback packetCallback,
                                      StatusCallback statusCallback) {
    auto impl = std::make_shared<Impl>(loop, std::move(packetCallback),
                                       std::move(statusCallback));
    impl->m_exec = UvExecFunc::Create(loop);
    if (!impl->m_exec) {
      return nullptr;
    }
    impl->m_exec->wakeup.connect([](auto func) { func(); });
    return impl;
  }

  Impl(uv::Loop& loop, PacketCallback packetCallback,
       StatusCallback statusCallback)
      : m_loop{loop},
        m_packetCallback{std::move(packetCallback)},
        m_statusCallback{std::move(statusCallback)} {
    m_status.supported = true;
    m_status.status = "Waiting for Bluetooth address";
  }

  ~Impl() { Disconnect("Disconnected"); }

  bool Connect(BluetoothL2CAPClientConfig config) {
    if (config.address.empty()) {
      SetError("No Bluetooth address configured");
      return false;
    }
    if (config.gattServiceUuid.empty() ||
        config.gattControlCharacteristicUuid.empty() ||
        config.gattStatusCharacteristicUuid.empty()) {
      SetError("No Bluetooth GATT UUIDs configured");
      return false;
    }

    uint64_t bluetoothAddress = 0;
    if (!ParseBluetoothAddress(config.address, &bluetoothAddress)) {
      SetError(
          "Windows GATT requires a Bluetooth address of the form "
          "AA:BB:CC:DD:EE:FF");
      return false;
    }

    {
      std::scoped_lock lock{m_statusMutex};
      m_config = config;
      m_status.targetAddress = config.address;
      m_status.addressType = config.addressType;
      m_status.targetConfigured = true;
      m_status.connecting = true;
      m_status.connected = false;
      m_status.transport = BluetoothPacketTransport::NONE;
      m_status.error.clear();
      m_status.status = "Connecting (GATT)";
    }
    PublishStatus();

    Disconnect({});
    m_cancelConnect = false;
    auto self = shared_from_this();
    m_connectThread =
        std::thread{[self, config = std::move(config), bluetoothAddress] {
          self->ConnectThreadMain(config, bluetoothAddress);
        }};
    return true;
  }

  void Disconnect(std::string_view reason) {
    m_cancelConnect = true;
    if (m_connectThread.joinable()) {
      m_connectThread.join();
    }

    {
      std::scoped_lock lock{m_gattMutex};
      if (m_statusCharacteristic && m_valueChangedToken.value != 0) {
        m_statusCharacteristic.ValueChanged(m_valueChangedToken);
        m_valueChangedToken = {};
      }
      m_controlCharacteristic = nullptr;
      m_statusCharacteristic = nullptr;
      m_device = nullptr;
    }

    if (!reason.empty()) {
      UpdateStatus([&](auto& status) {
        status.connecting = false;
        status.connected = false;
        status.transport = BluetoothPacketTransport::NONE;
        status.status = reason;
      });
    }
  }

  bool Send(std::span<const uint8_t> packet) {
    if (packet.empty()) {
      return false;
    }

    gatt::GattCharacteristic controlCharacteristic{nullptr};
    bool tooLarge = false;
    {
      std::scoped_lock lock{m_statusMutex};
      if (!m_status.connected) {
        return false;
      }
      tooLarge = packet.size() > m_config.maxPacketSize;
    }
    if (tooLarge) {
      SetError("Packet is larger than Bluetooth transport MTU");
      return false;
    }

    {
      std::scoped_lock lock{m_gattMutex};
      controlCharacteristic = m_controlCharacteristic;
    }
    if (!controlCharacteristic) {
      return false;
    }

    try {
      auto status =
          controlCharacteristic
              .WriteValueAsync(ToBuffer(packet),
                               gatt::GattWriteOption::WriteWithoutResponse)
              .get();
      if (status == gatt::GattCommunicationStatus::Success) {
        UpdateStatus([](auto& current) { ++current.packetsSent; });
        return true;
      }
      SetError("Bluetooth GATT write failed");
    } catch (winrt::hresult_error const& error) {
      SetError(std::string{"Bluetooth GATT write failed: "} + ToString(error));
    }
    return false;
  }

  BluetoothL2CAPConnectionStatus GetStatus() const {
    std::scoped_lock lock{m_statusMutex};
    return m_status;
  }

 private:
  void ConnectThreadMain(const BluetoothL2CAPClientConfig& config,
                         uint64_t bluetoothAddress) {
    try {
      winrt::init_apartment(winrt::apartment_type::multi_threaded);

      auto device =
          bt::BluetoothLEDevice::FromBluetoothAddressAsync(bluetoothAddress)
              .get();
      if (!device) {
        SetError("Bluetooth device was not found");
        return;
      }
      if (m_cancelConnect) {
        return;
      }

      winrt::guid serviceUuid = ParseUuid(config.gattServiceUuid);
      auto servicesResult =
          device
              .GetGattServicesForUuidAsync(serviceUuid,
                                           bt::BluetoothCacheMode::Uncached)
              .get();
      if (servicesResult.Status() != gatt::GattCommunicationStatus::Success ||
          servicesResult.Services().Size() == 0) {
        SetError("Bluetooth GATT service was not found");
        return;
      }

      auto service = servicesResult.Services().GetAt(0);
      winrt::guid controlUuid = ParseUuid(config.gattControlCharacteristicUuid);
      winrt::guid statusUuid = ParseUuid(config.gattStatusCharacteristicUuid);

      auto controlResult =
          service
              .GetCharacteristicsForUuidAsync(controlUuid,
                                              bt::BluetoothCacheMode::Uncached)
              .get();
      auto statusResult = service
                              .GetCharacteristicsForUuidAsync(
                                  statusUuid, bt::BluetoothCacheMode::Uncached)
                              .get();
      if (controlResult.Status() != gatt::GattCommunicationStatus::Success ||
          statusResult.Status() != gatt::GattCommunicationStatus::Success ||
          controlResult.Characteristics().Size() == 0 ||
          statusResult.Characteristics().Size() == 0) {
        SetError("Bluetooth GATT packet characteristics were not found");
        return;
      }

      auto controlCharacteristic = controlResult.Characteristics().GetAt(0);
      auto statusCharacteristic = statusResult.Characteristics().GetAt(0);

      auto self = shared_from_this();
      auto token = statusCharacteristic.ValueChanged(
          [self](gatt::GattCharacteristic const&,
                 gatt::GattValueChangedEventArgs const& args) {
            self->DidReceivePacket(args.CharacteristicValue());
          });

      auto notifyStatus =
          statusCharacteristic
              .WriteClientCharacteristicConfigurationDescriptorAsync(
                  gatt::GattClientCharacteristicConfigurationDescriptorValue::
                      Notify)
              .get();
      if (notifyStatus != gatt::GattCommunicationStatus::Success) {
        statusCharacteristic.ValueChanged(token);
        SetError("Failed to enable Bluetooth GATT notifications");
        return;
      }

      {
        std::scoped_lock lock{m_gattMutex};
        m_device = device;
        m_controlCharacteristic = controlCharacteristic;
        m_statusCharacteristic = statusCharacteristic;
        m_valueChangedToken = token;
      }

      UpdateStatus([](auto& status) {
        status.connecting = false;
        status.connected = true;
        status.transport = BluetoothPacketTransport::GATT;
        status.status = "Connected (GATT)";
        status.error.clear();
      });
    } catch (winrt::hresult_error const& error) {
      SetError(std::string{"Bluetooth GATT connection failed: "} +
               ToString(error));
    }
  }

  void DidReceivePacket(streams::IBuffer const& buffer) {
    std::vector<uint8_t> packet = FromBuffer(buffer);
    if (packet.empty()) {
      return;
    }
    UpdateStatus([](auto& status) { ++status.packetsReceived; });
    if (m_packetCallback) {
      m_exec->Send([callback = m_packetCallback, packet = std::move(packet)] {
        callback(packet);
      });
    }
  }

  void SetError(std::string_view error) {
    UpdateStatus([&](auto& status) {
      status.error = error;
      status.status = error;
      status.connecting = false;
      status.connected = false;
      status.transport = BluetoothPacketTransport::NONE;
    });
  }

  void PublishStatus() {
    BluetoothL2CAPConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      snapshot = m_status;
    }
    if (m_statusCallback) {
      m_statusCallback(snapshot);
    }
  }

  template <typename F>
  void UpdateStatus(F&& func) {
    BluetoothL2CAPConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      func(m_status);
      snapshot = m_status;
    }
    if (m_statusCallback) {
      m_exec->Send(
          [callback = m_statusCallback, snapshot] { callback(snapshot); });
    }
  }

  uv::Loop& m_loop;
  PacketCallback m_packetCallback;
  StatusCallback m_statusCallback;
  std::shared_ptr<UvExecFunc> m_exec;

  mutable std::mutex m_statusMutex;
  BluetoothL2CAPConnectionStatus m_status;
  BluetoothL2CAPClientConfig m_config;

  std::mutex m_gattMutex;
  bt::BluetoothLEDevice m_device{nullptr};
  gatt::GattCharacteristic m_controlCharacteristic{nullptr};
  gatt::GattCharacteristic m_statusCharacteristic{nullptr};
  winrt::event_token m_valueChangedToken{};

  std::atomic_bool m_cancelConnect{false};
  std::thread m_connectThread;
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
  return true;
}

bool BluetoothL2CAPClient::Connect(BluetoothL2CAPClientConfig config) {
  config.preferL2CAP = false;
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
