// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "wpi/net/BluetoothLEPacketClient.hpp"

#include <Windows.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <format>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <combaseapi.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"

#pragma comment(lib, "windowsapp")

namespace uv = wpi::net::uv;
namespace bt = winrt::Windows::Devices::Bluetooth;
namespace dev = winrt::Windows::Devices::Enumeration;
namespace gatt = winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
namespace streams = winrt::Windows::Storage::Streams;

using namespace wpi::net;

namespace {

constexpr const wchar_t* BLUETOOTH_DEVICE_ADDRESS_PROPERTY =
    L"System.Devices.Aep.DeviceAddress";

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

std::string FormatBluetoothAddress(uint64_t address) {
  char formatted[18];
  std::snprintf(formatted, sizeof(formatted), "%02X:%02X:%02X:%02X:%02X:%02X",
                static_cast<unsigned>((address >> 40) & 0xff),
                static_cast<unsigned>((address >> 32) & 0xff),
                static_cast<unsigned>((address >> 24) & 0xff),
                static_cast<unsigned>((address >> 16) & 0xff),
                static_cast<unsigned>((address >> 8) & 0xff),
                static_cast<unsigned>(address & 0xff));
  return formatted;
}

std::string ExtractBluetoothAddress(std::string_view value) {
  for (size_t i = 0; i + 17 <= value.size(); ++i) {
    uint64_t address = 0;
    if (ParseBluetoothAddress(value.substr(i, 17), &address)) {
      return FormatBluetoothAddress(address);
    }
  }

  for (size_t i = 0; i + 12 <= value.size(); ++i) {
    if (i > 0 && HexDigit(value[i - 1]) != 0xff) {
      continue;
    }
    if (i + 12 < value.size() && HexDigit(value[i + 12]) != 0xff) {
      continue;
    }

    uint64_t address = 0;
    bool valid = true;
    for (size_t j = 0; j < 12; ++j) {
      uint8_t digit = HexDigit(value[i + j]);
      if (digit == 0xff) {
        valid = false;
        break;
      }
      address = (address << 4) | digit;
    }
    if (valid) {
      return FormatBluetoothAddress(address);
    }
  }

  return {};
}

void SortBluetoothDevices(std::vector<BluetoothLEDeviceInfo>* devices) {
  std::stable_sort(devices->begin(), devices->end(),
                   [](const auto& a, const auto& b) {
                     std::string_view aKey =
                         a.name.empty() ? std::string_view{a.target} : a.name;
                     std::string_view bKey =
                         b.name.empty() ? std::string_view{b.target} : b.name;
                     if (aKey != bKey) {
                       return aKey < bKey;
                     }
                     return a.target < b.target;
                   });
}

winrt::Windows::Foundation::Collections::IVector<winrt::hstring>
GetBluetoothDeviceProperties() {
  return winrt::single_threaded_vector<winrt::hstring>(
      {winrt::hstring{BLUETOOTH_DEVICE_ADDRESS_PROPERTY}});
}

std::string GetDeviceTarget(dev::DeviceInformation const& info) {
  try {
    auto properties = info.Properties();
    winrt::hstring addressProperty{BLUETOOTH_DEVICE_ADDRESS_PROPERTY};
    if (properties.HasKey(addressProperty)) {
      auto address = properties.Lookup(addressProperty);
      std::string target = ExtractBluetoothAddress(
          winrt::to_string(winrt::unbox_value<winrt::hstring>(address)));
      if (!target.empty()) {
        return target;
      }
    }
  } catch (winrt::hresult_error const&) {
  }

  return ExtractBluetoothAddress(winrt::to_string(info.Id()));
}

bool DeviceMatchesTarget(dev::DeviceInformation const& info,
                         std::string_view target) {
  std::string deviceTarget = GetDeviceTarget(info);
  if (!deviceTarget.empty() && deviceTarget == target) {
    return true;
  }
  return winrt::to_string(info.Id()) == target ||
         winrt::to_string(info.Name()) == target;
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

bt::BluetoothAddressType ToWinrtBluetoothAddressType(
    BluetoothAddressType addressType) {
  return addressType == BluetoothAddressType::PUBLIC
             ? bt::BluetoothAddressType::Public
             : bt::BluetoothAddressType::Random;
}

std::string ToString(winrt::hresult_error const& error) {
  std::string result = winrt::to_string(error.message());
  if (result.empty()) {
    result =
        std::format("HRESULT 0x{:08X}", static_cast<uint32_t>(error.code()));
  }
  return result;
}

void EnsureWinrtApartment() {
  APTTYPE type;
  APTTYPEQUALIFIER qualifier;
  HRESULT hr = ::CoGetApartmentType(&type, &qualifier);
  if (SUCCEEDED(hr)) {
    return;
  }
  if (hr == CO_E_NOTINITIALIZED) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    return;
  }
  winrt::throw_hresult(hr);
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

std::vector<dev::DeviceInformation> FindKnownBluetoothDevices() {
  auto devices = dev::DeviceInformation::FindAllAsync(
                     bt::BluetoothLEDevice::GetDeviceSelector(),
                     GetBluetoothDeviceProperties())
                     .get();
  std::vector<dev::DeviceInformation> result;
  result.reserve(devices.Size());
  for (auto const& info : devices) {
    result.emplace_back(info);
  }
  return result;
}

std::vector<dev::DeviceInformation> FindUnpairedBluetoothDevices() {
  auto devices =
      dev::DeviceInformation::FindAllAsync(
          bt::BluetoothLEDevice::GetDeviceSelectorFromPairingState(false),
          GetBluetoothDeviceProperties())
          .get();
  std::vector<dev::DeviceInformation> result;
  result.reserve(devices.Size());
  for (auto const& info : devices) {
    result.emplace_back(info);
  }
  return result;
}

void AppendUniqueDevice(std::vector<dev::DeviceInformation>* devices,
                        dev::DeviceInformation const& info) {
  std::string id = winrt::to_string(info.Id());
  auto duplicate =
      std::find_if(devices->begin(), devices->end(), [&](auto const& existing) {
        return winrt::to_string(existing.Id()) == id;
      });
  if (duplicate == devices->end()) {
    devices->emplace_back(info);
  }
}

struct BluetoothDeviceScanState {
  std::vector<dev::DeviceInformation> devices;
  std::vector<std::string> deviceIds;
  std::mutex mutex;
  std::condition_variable cv;
  bool stopped = false;
};

std::vector<dev::DeviceInformation> ScanBluetoothDevices(
    std::chrono::milliseconds timeout) {
  auto state = std::make_shared<BluetoothDeviceScanState>();

  auto watcher = dev::DeviceInformation::CreateWatcher(
      bt::BluetoothLEDevice::GetDeviceSelectorFromPairingState(false),
      GetBluetoothDeviceProperties());
  auto addedToken = watcher.Added(
      [state](dev::DeviceWatcher const&, dev::DeviceInformation const& info) {
        std::scoped_lock lock{state->mutex};
        std::string id = winrt::to_string(info.Id());
        if (std::find(state->deviceIds.begin(), state->deviceIds.end(), id) !=
            state->deviceIds.end()) {
          return;
        }
        state->deviceIds.emplace_back(std::move(id));
        state->devices.emplace_back(info);
      });
  auto stoppedToken =
      watcher.Stopped([state](dev::DeviceWatcher const&,
                              winrt::Windows::Foundation::IInspectable const&) {
        {
          std::scoped_lock lock{state->mutex};
          state->stopped = true;
        }
        state->cv.notify_all();
      });

  watcher.Start();
  {
    std::unique_lock lock{state->mutex};
    state->cv.wait_for(lock, timeout, [&] { return state->stopped; });
  }

  auto status = watcher.Status();
  if (status == dev::DeviceWatcherStatus::Started ||
      status == dev::DeviceWatcherStatus::EnumerationCompleted) {
    watcher.Stop();
  }
  if (status == dev::DeviceWatcherStatus::Started ||
      status == dev::DeviceWatcherStatus::EnumerationCompleted ||
      status == dev::DeviceWatcherStatus::Stopping) {
    std::unique_lock lock{state->mutex};
    state->cv.wait_for(lock, std::chrono::seconds{2},
                       [&] { return state->stopped; });
  }

  watcher.Added(addedToken);
  watcher.Stopped(stoppedToken);

  std::scoped_lock lock{state->mutex};
  return state->devices;
}

}  // namespace

class BluetoothLEPacketClient::Impl
    : public std::enable_shared_from_this<BluetoothLEPacketClient::Impl> {
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

  bool Connect(BluetoothLEPacketClientConfig config) {
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
    uint64_t generation = ++m_connectGeneration;
    auto self = shared_from_this();
    m_connectThread = std::thread{
        [self, config = std::move(config), bluetoothAddress, generation] {
          self->ConnectThreadMain(config, bluetoothAddress, generation);
        }};
    return true;
  }

  void Disconnect(std::string_view reason) {
    m_cancelConnect = true;
    ++m_connectGeneration;
    if (m_connectThread.joinable()) {
      // WinRT GATT discovery calls are synchronous and cannot be interrupted by
      // m_cancelConnect. Detach so GUI disconnect/shutdown can return promptly;
      // the generation checks in ConnectThreadMain drop late results.
      m_connectThread.detach();
    }

    ClearGattState();

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
      FailConnectedGatt("Packet is larger than Bluetooth transport MTU");
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
      EnsureWinrtApartment();
      auto status =
          controlCharacteristic
              .WriteValueAsync(ToBuffer(packet),
                               gatt::GattWriteOption::WriteWithoutResponse)
              .get();
      if (status == gatt::GattCommunicationStatus::Success) {
        UpdateStatus([](auto& current) { ++current.packetsSent; });
        return true;
      }
      FailConnectedGatt("Bluetooth GATT write failed");
    } catch (winrt::hresult_error const& error) {
      FailConnectedGatt(
          std::format("Bluetooth GATT write failed: {}", ToString(error)));
    }
    return false;
  }

  BluetoothLEPacketConnectionStatus GetStatus() const {
    std::scoped_lock lock{m_statusMutex};
    return m_status;
  }

 private:
  bool IsConnectCanceled(uint64_t generation) const {
    return m_cancelConnect ||
           m_connectGeneration.load(std::memory_order_acquire) != generation;
  }

  void RemoveGattValueChanged(gatt::GattCharacteristic const& characteristic,
                              winrt::event_token token) {
    if (characteristic && token.value != 0) {
      try {
        EnsureWinrtApartment();
        characteristic.ValueChanged(token);
      } catch (winrt::hresult_error const&) {
      }
    }
  }

  void ConnectThreadMain(const BluetoothLEPacketClientConfig& config,
                         uint64_t bluetoothAddress, uint64_t generation) {
    try {
      EnsureWinrtApartment();

      auto device =
          bt::BluetoothLEDevice::FromBluetoothAddressAsync(
              bluetoothAddress, ToWinrtBluetoothAddressType(config.addressType))
              .get();
      if (IsConnectCanceled(generation)) {
        return;
      }
      if (!device) {
        SetError("Bluetooth device was not found");
        return;
      }

      winrt::guid serviceUuid = ParseUuid(config.gattServiceUuid);
      auto servicesResult =
          device
              .GetGattServicesForUuidAsync(serviceUuid,
                                           bt::BluetoothCacheMode::Uncached)
              .get();
      if (IsConnectCanceled(generation)) {
        return;
      }
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
      if (IsConnectCanceled(generation)) {
        return;
      }
      auto statusResult = service
                              .GetCharacteristicsForUuidAsync(
                                  statusUuid, bt::BluetoothCacheMode::Uncached)
                              .get();
      if (IsConnectCanceled(generation)) {
        return;
      }
      if (controlResult.Status() != gatt::GattCommunicationStatus::Success ||
          statusResult.Status() != gatt::GattCommunicationStatus::Success ||
          controlResult.Characteristics().Size() == 0 ||
          statusResult.Characteristics().Size() == 0) {
        SetError("Bluetooth GATT packet characteristics were not found");
        return;
      }

      auto controlCharacteristic = controlResult.Characteristics().GetAt(0);
      auto statusCharacteristic = statusResult.Characteristics().GetAt(0);

      if (IsConnectCanceled(generation)) {
        return;
      }
      auto weak = weak_from_this();
      auto token = statusCharacteristic.ValueChanged(
          [weak, generation](gatt::GattCharacteristic const&,
                             gatt::GattValueChangedEventArgs const& args) {
            if (auto self = weak.lock();
                self && !self->IsConnectCanceled(generation)) {
              self->DidReceivePacket(args.CharacteristicValue());
            }
          });

      auto notifyStatus =
          statusCharacteristic
              .WriteClientCharacteristicConfigurationDescriptorAsync(
                  gatt::GattClientCharacteristicConfigurationDescriptorValue::
                      Notify)
              .get();
      if (IsConnectCanceled(generation)) {
        RemoveGattValueChanged(statusCharacteristic, token);
        return;
      }
      if (notifyStatus != gatt::GattCommunicationStatus::Success) {
        RemoveGattValueChanged(statusCharacteristic, token);
        SetError("Failed to enable Bluetooth GATT notifications");
        return;
      }

      {
        std::scoped_lock lock{m_gattMutex};
        if (IsConnectCanceled(generation)) {
          RemoveGattValueChanged(statusCharacteristic, token);
          return;
        }
        m_device = device;
        m_controlCharacteristic = controlCharacteristic;
        m_statusCharacteristic = statusCharacteristic;
        m_valueChangedToken = token;
      }

      UpdateStatus([&](auto& status) {
        if (IsConnectCanceled(generation)) {
          return;
        }
        status.connecting = false;
        status.connected = true;
        status.transport = BluetoothPacketTransport::GATT;
        status.status = "Connected (GATT)";
        status.error.clear();
      });
    } catch (winrt::hresult_error const& error) {
      if (!IsConnectCanceled(generation)) {
        SetError(std::format("Bluetooth GATT connection failed: {}",
                             ToString(error)));
      }
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

  void ClearGattState() {
    std::scoped_lock lock{m_gattMutex};
    RemoveGattValueChanged(m_statusCharacteristic, m_valueChangedToken);
    m_valueChangedToken = {};
    m_controlCharacteristic = nullptr;
    m_statusCharacteristic = nullptr;
    m_device = nullptr;
  }

  void FailConnectedGatt(std::string_view error) {
    ClearGattState();
    SetError(error);
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
    BluetoothLEPacketConnectionStatus snapshot;
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
    BluetoothLEPacketConnectionStatus snapshot;
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
  BluetoothLEPacketConnectionStatus m_status;
  BluetoothLEPacketClientConfig m_config;

  std::mutex m_gattMutex;
  bt::BluetoothLEDevice m_device{nullptr};
  gatt::GattCharacteristic m_controlCharacteristic{nullptr};
  gatt::GattCharacteristic m_statusCharacteristic{nullptr};
  winrt::event_token m_valueChangedToken{};

  std::atomic_bool m_cancelConnect{false};
  std::atomic<uint64_t> m_connectGeneration{0};
  std::thread m_connectThread;
};

std::shared_ptr<BluetoothLEPacketClient> BluetoothLEPacketClient::Create(
    wpi::net::uv::Loop& loop, PacketCallback packetCallback,
    StatusCallback statusCallback) {
  auto impl =
      Impl::Create(loop, std::move(packetCallback), std::move(statusCallback));
  if (!impl) {
    return nullptr;
  }
  return std::shared_ptr<BluetoothLEPacketClient>(
      new BluetoothLEPacketClient{std::move(impl)});
}

BluetoothLEPacketClient::BluetoothLEPacketClient(std::shared_ptr<Impl> impl)
    : m_impl{std::move(impl)} {}

BluetoothLEPacketClient::~BluetoothLEPacketClient() {
  if (m_impl) {
    m_impl->Disconnect({});
  }
}

bool BluetoothLEPacketClient::IsPairingSupported() {
  return true;
}

BluetoothLEDeviceScanResult BluetoothLEPacketClient::ScanDevices(
    std::chrono::milliseconds timeout) {
  BluetoothLEDeviceScanResult result;
  result.supported = true;
  try {
    EnsureWinrtApartment();
    auto devices = FindKnownBluetoothDevices();
    if (timeout > std::chrono::milliseconds{0}) {
      for (auto const& info : ScanBluetoothDevices(timeout)) {
        AppendUniqueDevice(&devices, info);
      }
    }
    for (auto const& info : devices) {
      std::string target = GetDeviceTarget(info);
      if (target.empty()) {
        continue;
      }

      BluetoothLEDeviceInfo device;
      device.target = std::move(target);
      device.name = winrt::to_string(info.Name());
      device.addressType = BluetoothAddressType::RANDOM;
      device.paired = info.Pairing().IsPaired();
      device.pairable = info.Pairing().CanPair();
      result.devices.emplace_back(std::move(device));
    }

    SortBluetoothDevices(&result.devices);
    result.status = std::format("Discovered {} Bluetooth LE devices",
                                result.devices.size());
  } catch (winrt::hresult_error const& error) {
    result.error =
        std::format("Bluetooth LE device scan failed: {}", ToString(error));
  }
  return result;
}

BluetoothLEPairingResult BluetoothLEPacketClient::PairDevice(
    std::string_view target) {
  BluetoothLEPairingResult result;
  result.supported = true;
  try {
    EnsureWinrtApartment();
    auto devices = FindKnownBluetoothDevices();
    for (auto const& info : FindUnpairedBluetoothDevices()) {
      AppendUniqueDevice(&devices, info);
    }
    for (auto const& info : devices) {
      if (!DeviceMatchesTarget(info, target)) {
        continue;
      }

      if (info.Pairing().IsPaired()) {
        result.paired = true;
        result.status = "Bluetooth device is already paired";
        return result;
      }
      if (!info.Pairing().CanPair()) {
        result.error = "Bluetooth device is not pairable";
        return result;
      }

      auto pairResult = info.Pairing().PairAsync().get();
      auto status = pairResult.Status();
      result.paired = status == dev::DevicePairingResultStatus::Paired ||
                      status == dev::DevicePairingResultStatus::AlreadyPaired;
      if (result.paired) {
        result.status = "Bluetooth device paired";
      } else {
        result.error = "Bluetooth pairing failed";
      }
      return result;
    }

    result.error = "Bluetooth device was not found";
  } catch (winrt::hresult_error const& error) {
    result.error = std::format("Bluetooth pairing failed: {}", ToString(error));
  }
  return result;
}

bool BluetoothLEPacketClient::Connect(BluetoothLEPacketClientConfig config) {
  config.preferL2CAP = false;
  return m_impl->Connect(std::move(config));
}

void BluetoothLEPacketClient::Disconnect(std::string_view reason) {
  m_impl->Disconnect(reason);
}

bool BluetoothLEPacketClient::Send(std::span<const uint8_t> packet) {
  return m_impl->Send(packet);
}

BluetoothLEPacketConnectionStatus BluetoothLEPacketClient::GetStatus() const {
  return m_impl->GetStatus();
}
