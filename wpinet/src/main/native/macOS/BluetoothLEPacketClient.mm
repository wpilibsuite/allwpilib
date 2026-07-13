// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothLEPacketClient.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <utility>
#include <vector>

#import <CoreBluetooth/CoreBluetooth.h>
#import <Foundation/Foundation.h>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"

namespace uv = wpi::net::uv;

using namespace wpi::net;

@class WPINetMacBluetoothLEPacketClient;

class BluetoothLEPacketClient::Impl
    : public std::enable_shared_from_this<BluetoothLEPacketClient::Impl> {
 public:
  using LoopFunc = std::function<void()>;
  using UvExecFunc = uv::Async<LoopFunc>;

  static std::shared_ptr<Impl> Create(uv::Loop& loop,
                                      PacketCallback packetCallback,
                                      StatusCallback statusCallback);

  Impl(uv::Loop& loop, PacketCallback packetCallback,
       StatusCallback statusCallback);
  ~Impl();

  bool Connect(BluetoothLEPacketClientConfig config);
  void Disconnect(std::string_view reason);
  bool Send(std::span<const uint8_t> packet);
  BluetoothLEPacketConnectionStatus GetStatus() const;

  void SetStatus(std::string_view status);
  void SetError(std::string_view error);
  void SetConnected(BluetoothPacketTransport transport);
  void SetDisconnected(std::string_view reason);
  void DidReceivePacket(std::span<const uint8_t> packet);
  void DidSendPacket();

 private:
  template <typename F>
  void UpdateStatus(F&& func) {
    BluetoothLEPacketConnectionStatus snapshot;
    {
      std::scoped_lock lock{m_statusMutex};
      func(m_status);
      snapshot = m_status;
    }
    PostStatus(snapshot);
  }

  void PostStatus(const BluetoothLEPacketConnectionStatus& status);

  PacketCallback m_packetCallback;
  StatusCallback m_statusCallback;
  std::shared_ptr<UvExecFunc> m_exec;

  __strong WPINetMacBluetoothLEPacketClient* m_client = nil;

  mutable std::mutex m_statusMutex;
  BluetoothLEPacketConnectionStatus m_status;
  BluetoothLEPacketClientConfig m_config;
};

namespace {

char MAC_BLUETOOTH_QUEUE_KEY;

struct MacBluetoothLEPacketClientBridge {
  void* context = nullptr;
  void (*setStatus)(void* context, std::string_view status) = nullptr;
  void (*setError)(void* context, std::string_view error) = nullptr;
  void (*setConnected)(void* context, BluetoothPacketTransport transport) =
      nullptr;
  void (*setDisconnected)(void* context, std::string_view reason) = nullptr;
  void (*didReceivePacket)(void* context, std::span<const uint8_t> packet) =
      nullptr;
  void (*didSendPacket)(void* context) = nullptr;

  explicit operator bool() const { return context != nullptr; }

  void SetStatus(std::string_view status) const {
    if (setStatus != nullptr) {
      setStatus(context, status);
    }
  }

  void SetError(std::string_view error) const {
    if (setError != nullptr) {
      setError(context, error);
    }
  }

  void SetConnected(BluetoothPacketTransport transport) const {
    if (setConnected != nullptr) {
      setConnected(context, transport);
    }
  }

  void SetDisconnected(std::string_view reason) const {
    if (setDisconnected != nullptr) {
      setDisconnected(context, reason);
    }
  }

  void DidReceivePacket(std::span<const uint8_t> packet) const {
    if (didReceivePacket != nullptr) {
      didReceivePacket(context, packet);
    }
  }

  void DidSendPacket() const {
    if (didSendPacket != nullptr) {
      didSendPacket(context);
    }
  }
};

NSString* ToNSString(std::string_view value) {
  return [[NSString alloc] initWithBytes:value.data()
                                  length:value.size()
                                encoding:NSUTF8StringEncoding];
}

std::string ToString(NSString* value) {
  if (value == nil) {
    return {};
  }
  const char* utf8 = value.UTF8String;
  return utf8 != nullptr ? std::string{utf8} : std::string{};
}

std::string ToString(NSError* error) {
  if (error == nil) {
    return {};
  }
  return ToString(error.localizedDescription);
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

}  // namespace

@interface WPINetMacBluetoothLEDeviceScanner : NSObject <CBCentralManagerDelegate>
- (BluetoothLEDeviceScanResult)scanForDuration:(NSTimeInterval)duration;
@end

@implementation WPINetMacBluetoothLEDeviceScanner {
  dispatch_queue_t _queue;
  CBCentralManager* _central;
  dispatch_semaphore_t _done;
  BOOL _finished;
  BOOL _startedScan;
  NSTimeInterval _duration;
  std::vector<BluetoothLEDeviceInfo> _devices;
  std::string _error;
}

- (instancetype)init {
  self = [super init];
  if (self != nil) {
    _queue = dispatch_queue_create("edu.wpi.first.wpinet.bluetooth.scan",
                                   DISPATCH_QUEUE_SERIAL);
  }
  return self;
}

- (BluetoothLEDeviceScanResult)scanForDuration:(NSTimeInterval)duration {
  _duration = duration > 0.0 ? duration : 2.0;
  _done = dispatch_semaphore_create(0);
  _finished = NO;
  _startedScan = NO;
  _devices.clear();
  _error.clear();
  _central = [[CBCentralManager alloc] initWithDelegate:self queue:_queue];

  dispatch_time_t deadline = dispatch_time(
      DISPATCH_TIME_NOW, static_cast<int64_t>((_duration + 5.0) * NSEC_PER_SEC));
  dispatch_semaphore_wait(_done, deadline);

  BluetoothLEDeviceScanResult result;
  result.supported = true;
  dispatch_sync(_queue, ^{
    [self finishLocked];
  });
  result.devices = _devices;
  result.error = _error;

  SortBluetoothDevices(&result.devices);
  if (result.error.empty()) {
    result.status =
        std::format("Discovered {} Bluetooth LE devices", result.devices.size());
  }
  return result;
}

- (void)finishLocked {
  if (_finished) {
    return;
  }
  _finished = YES;
  [_central stopScan];
  _central.delegate = nil;
  _central = nil;
  dispatch_semaphore_signal(_done);
}

- (void)centralManagerDidUpdateState:(CBCentralManager*)central {
  if (central.state == CBManagerStatePoweredOn) {
    if (_startedScan) {
      return;
    }
    _startedScan = YES;
    [_central scanForPeripheralsWithServices:nil
                                     options:@{
                                       CBCentralManagerScanOptionAllowDuplicatesKey : @NO
                                     }];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,
                                 static_cast<int64_t>(_duration * NSEC_PER_SEC)),
                   _queue, ^{
      [self finishLocked];
    });
    return;
  }

  if (central.state == CBManagerStatePoweredOff) {
    _error = "Bluetooth is powered off";
    [self finishLocked];
  } else if (central.state == CBManagerStateUnsupported) {
    _error = "Bluetooth LE is unsupported on this Mac";
    [self finishLocked];
  } else if (central.state == CBManagerStateUnauthorized) {
    _error = "Bluetooth permission was denied";
    [self finishLocked];
  }
}

- (void)centralManager:(CBCentralManager*)central
    didDiscoverPeripheral:(CBPeripheral*)peripheral
        advertisementData:(NSDictionary<NSString*, id>*)advertisementData
                     RSSI:(NSNumber*)RSSI {
  (void)central;
  (void)RSSI;

  BluetoothLEDeviceInfo device;
  device.target = ToString(peripheral.identifier.UUIDString);
  NSString* advertisedName = advertisementData[CBAdvertisementDataLocalNameKey];
  NSString* name = advertisedName.length > 0 ? advertisedName : peripheral.name;
  device.name = ToString(name);
  device.addressType = BluetoothAddressType::RANDOM;
  device.pairable = false;

  auto it = std::find_if(_devices.begin(), _devices.end(), [&](const auto& d) {
    return d.target == device.target;
  });
  if (it == _devices.end()) {
    _devices.emplace_back(std::move(device));
  } else if (it->name.empty() && !device.name.empty()) {
    it->name = std::move(device.name);
  }
}

@end

@interface WPINetMacBluetoothLEPacketClient
    : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>
- (instancetype)initWithBridge:(MacBluetoothLEPacketClientBridge)bridge;
- (void)connectWithTarget:(NSString*)target
              serviceUuid:(NSString*)serviceUuid
              controlUuid:(NSString*)controlUuid
               statusUuid:(NSString*)statusUuid
            maxPacketSize:(NSUInteger)maxPacketSize;
- (void)disconnectWithReason:(NSString*)reason;
- (void)sendPacket:(NSData*)packet;
- (void)invalidate;
@end

@implementation WPINetMacBluetoothLEPacketClient {
  MacBluetoothLEPacketClientBridge _bridge;
  dispatch_queue_t _queue;
  CBCentralManager* _central;
  CBPeripheral* _peripheral;
  CBCharacteristic* _controlCharacteristic;
  CBCharacteristic* _statusCharacteristic;
  NSString* _target;
  CBUUID* _serviceUuid;
  CBUUID* _controlUuid;
  CBUUID* _statusUuid;
  NSUInteger _maxPacketSize;
}

- (instancetype)initWithBridge:(MacBluetoothLEPacketClientBridge)bridge {
  self = [super init];
  if (self != nil) {
    _bridge = bridge;
    _queue =
        dispatch_queue_create("edu.wpi.first.wpinet.bluetooth", DISPATCH_QUEUE_SERIAL);
    dispatch_queue_set_specific(_queue, &MAC_BLUETOOTH_QUEUE_KEY,
                                (__bridge void*)self, nullptr);
    _central = [[CBCentralManager alloc] initWithDelegate:self queue:_queue];
    _maxPacketSize = 512;
  }
  return self;
}

- (void)connectWithTarget:(NSString*)target
              serviceUuid:(NSString*)serviceUuid
              controlUuid:(NSString*)controlUuid
               statusUuid:(NSString*)statusUuid
            maxPacketSize:(NSUInteger)maxPacketSize {
  dispatch_async(_queue, ^{
    _target = [target copy];
    _serviceUuid = [CBUUID UUIDWithString:serviceUuid];
    _controlUuid = [CBUUID UUIDWithString:controlUuid];
    _statusUuid = [CBUUID UUIDWithString:statusUuid];
    _maxPacketSize = maxPacketSize;
    _controlCharacteristic = nil;
    _statusCharacteristic = nil;
    if (_peripheral != nil) {
      [_central cancelPeripheralConnection:_peripheral];
      _peripheral = nil;
    }
    [self startConnectIfReady];
  });
}

- (void)disconnectWithReason:(NSString*)reason {
  dispatch_async(_queue, ^{
    [_central stopScan];
    if (_peripheral != nil) {
      [_central cancelPeripheralConnection:_peripheral];
      _peripheral = nil;
    }
    _controlCharacteristic = nil;
    _statusCharacteristic = nil;
    if (_bridge) {
      _bridge.SetDisconnected(ToString(reason));
    }
  });
}

- (void)sendPacket:(NSData*)packet {
  dispatch_async(_queue, ^{
    if (_peripheral == nil || _controlCharacteristic == nil) {
      return;
    }
    NSUInteger maxWriteLength =
        [_peripheral maximumWriteValueLengthForType:
                         CBCharacteristicWriteWithoutResponse];
    if (packet.length > maxWriteLength) {
      if (_bridge) {
        _bridge.SetError("Packet is larger than Bluetooth GATT write MTU");
      }
      return;
    }
    [_peripheral writeValue:packet
          forCharacteristic:_controlCharacteristic
                       type:CBCharacteristicWriteWithoutResponse];
    if (_bridge) {
      _bridge.DidSendPacket();
    }
  });
}

- (void)invalidate {
  dispatch_block_t block = ^{
    [_central stopScan];
    if (_peripheral != nil) {
      [_central cancelPeripheralConnection:_peripheral];
      _peripheral = nil;
    }
    _central.delegate = nil;
    _bridge = {};
    _controlCharacteristic = nil;
    _statusCharacteristic = nil;
  };
  if (dispatch_get_specific(&MAC_BLUETOOTH_QUEUE_KEY) == (__bridge void*)self) {
    block();
  } else {
    dispatch_sync(_queue, block);
  }
}

- (void)startConnectIfReady {
  if (!_bridge) {
    return;
  }

  switch (_central.state) {
    case CBManagerStatePoweredOn:
      break;
    case CBManagerStatePoweredOff:
      _bridge.SetError("Bluetooth is powered off");
      return;
    case CBManagerStateUnsupported:
      _bridge.SetError("Bluetooth LE is unsupported on this Mac");
      return;
    case CBManagerStateUnauthorized:
      _bridge.SetError("Bluetooth permission was denied");
      return;
    default:
      _bridge.SetStatus("Waiting for Bluetooth to power on");
      return;
  }

  if (_target.length == 0) {
    _bridge.SetError("No Bluetooth target configured");
    return;
  }

  NSUUID* uuid = [[NSUUID alloc] initWithUUIDString:_target];
  if (uuid != nil) {
    NSArray<CBPeripheral*>* peripherals =
        [_central retrievePeripheralsWithIdentifiers:@[ uuid ]];
    if (peripherals.count > 0) {
      [self connectPeripheral:peripherals.firstObject];
      return;
    }
  }

  _bridge.SetStatus("Scanning for Bluetooth device");
  [_central scanForPeripheralsWithServices:nil
                                   options:@{
                                     CBCentralManagerScanOptionAllowDuplicatesKey : @NO
                                   }];
}

- (void)connectPeripheral:(CBPeripheral*)peripheral {
  if (!_bridge) {
    return;
  }
  [_central stopScan];
  _peripheral = peripheral;
  _bridge.SetStatus("Connecting");
  [_central connectPeripheral:peripheral options:nil];
}

- (BOOL)peripheralMatchesTarget:(CBPeripheral*)peripheral
              advertisementData:(NSDictionary<NSString*, id>*)advertisementData {
  if (_target.length == 0) {
    return NO;
  }

  NSString* identifier = peripheral.identifier.UUIDString;
  if ([identifier caseInsensitiveCompare:_target] == NSOrderedSame) {
    return YES;
  }

  NSString* advertisedName = advertisementData[CBAdvertisementDataLocalNameKey];
  NSString* name = advertisedName.length > 0 ? advertisedName : peripheral.name;
  if (name.length == 0) {
    return NO;
  }

  return [name caseInsensitiveCompare:_target] == NSOrderedSame ||
         [name hasPrefix:_target];
}

- (void)centralManagerDidUpdateState:(CBCentralManager*)central {
  (void)central;
  [self startConnectIfReady];
}

- (void)centralManager:(CBCentralManager*)central
    didDiscoverPeripheral:(CBPeripheral*)peripheral
        advertisementData:(NSDictionary<NSString*, id>*)advertisementData
                     RSSI:(NSNumber*)RSSI {
  (void)central;
  (void)RSSI;
  if ([self peripheralMatchesTarget:peripheral advertisementData:advertisementData]) {
    [self connectPeripheral:peripheral];
  }
}

- (void)centralManager:(CBCentralManager*)central
  didConnectPeripheral:(CBPeripheral*)peripheral {
  (void)central;
  if (peripheral != _peripheral) {
    return;
  }
  peripheral.delegate = self;
  // CoreBluetooth exposes L2CAP as NSStream, which does not preserve XRP packet
  // boundaries. Use GATT unless the protocol gains explicit stream framing.
  [self discoverGattService];
}

- (void)centralManager:(CBCentralManager*)central
    didFailToConnectPeripheral:(CBPeripheral*)peripheral
                         error:(NSError*)error {
  (void)central;
  if (peripheral != _peripheral) {
    return;
  }
  _peripheral = nil;
  _controlCharacteristic = nil;
  _statusCharacteristic = nil;
  if (_bridge) {
    _bridge.SetError(std::format("Failed to connect Bluetooth device: {}",
                                 ToString(error)));
  }
}

- (void)centralManager:(CBCentralManager*)central
    didDisconnectPeripheral:(CBPeripheral*)peripheral
                      error:(NSError*)error {
  (void)central;
  if (peripheral != _peripheral) {
    return;
  }
  _peripheral = nil;
  _controlCharacteristic = nil;
  _statusCharacteristic = nil;
  if (_bridge) {
    if (error != nil) {
      _bridge.SetError(
          std::format("Bluetooth connection closed: {}", ToString(error)));
    } else {
      _bridge.SetDisconnected("Bluetooth connection closed");
    }
  }
}

- (void)discoverGattService {
  if (_peripheral == nil || _serviceUuid == nil) {
    if (_bridge) {
      _bridge.SetError("No Bluetooth GATT service configured");
    }
    return;
  }
  if (_bridge) {
    _bridge.SetStatus("Discovering Bluetooth GATT service");
  }
  [_peripheral discoverServices:@[ _serviceUuid ]];
}

- (void)peripheral:(CBPeripheral*)peripheral didDiscoverServices:(NSError*)error {
  if (peripheral != _peripheral) {
    return;
  }
  if (error != nil) {
    if (_bridge) {
      _bridge.SetError(
          std::format("Failed to discover Bluetooth GATT service: {}",
                      ToString(error)));
    }
    return;
  }

  for (CBService* service in peripheral.services) {
    if ([service.UUID isEqual:_serviceUuid]) {
      if (_bridge) {
        _bridge.SetStatus("Discovering Bluetooth GATT characteristics");
      }
      [peripheral discoverCharacteristics:@[ _controlUuid, _statusUuid ]
                                forService:service];
      return;
    }
  }

  if (_bridge) {
    _bridge.SetError("Bluetooth GATT service was not found");
  }
}

- (void)peripheral:(CBPeripheral*)peripheral
    didDiscoverCharacteristicsForService:(CBService*)service
                                   error:(NSError*)error {
  (void)service;
  if (peripheral != _peripheral) {
    return;
  }
  if (error != nil) {
    if (_bridge) {
      _bridge.SetError(
          std::format("Failed to discover Bluetooth GATT characteristics: {}",
                      ToString(error)));
    }
    return;
  }

  _controlCharacteristic = nil;
  _statusCharacteristic = nil;
  for (CBCharacteristic* characteristic in service.characteristics) {
    if ([characteristic.UUID isEqual:_controlUuid]) {
      _controlCharacteristic = characteristic;
    } else if ([characteristic.UUID isEqual:_statusUuid]) {
      _statusCharacteristic = characteristic;
    }
  }

  if (_controlCharacteristic == nil || _statusCharacteristic == nil) {
    if (_bridge) {
      _bridge.SetError("Bluetooth GATT packet characteristics were not found");
    }
    return;
  }

  if (_bridge) {
    _bridge.SetStatus("Enabling Bluetooth GATT notifications");
  }
  [peripheral setNotifyValue:YES forCharacteristic:_statusCharacteristic];
}

- (void)peripheral:(CBPeripheral*)peripheral
    didUpdateNotificationStateForCharacteristic:(CBCharacteristic*)characteristic
                                          error:(NSError*)error {
  if (peripheral != _peripheral) {
    return;
  }
  if (![characteristic.UUID isEqual:_statusUuid]) {
    return;
  }
  if (error != nil) {
    if (_bridge) {
      _bridge.SetError(
          std::format("Failed to enable Bluetooth GATT notifications: {}",
                      ToString(error)));
    }
    return;
  }

  if (characteristic.isNotifying && _bridge) {
    _bridge.SetConnected(BluetoothPacketTransport::GATT);
  }
}

- (void)peripheral:(CBPeripheral*)peripheral
    didUpdateValueForCharacteristic:(CBCharacteristic*)characteristic
                              error:(NSError*)error {
  if (peripheral != _peripheral) {
    return;
  }
  if (![characteristic.UUID isEqual:_statusUuid]) {
    return;
  }
  if (error != nil) {
    if (_bridge) {
      _bridge.SetError(
          std::format("Bluetooth GATT notification failed: {}", ToString(error)));
    }
    return;
  }

  NSData* value = characteristic.value;
  if (value.length > 0 && _bridge) {
    _bridge.DidReceivePacket(
        {static_cast<const uint8_t*>(value.bytes), value.length});
  }
}

@end

std::shared_ptr<BluetoothLEPacketClient::Impl> BluetoothLEPacketClient::Impl::Create(
    uv::Loop& loop, PacketCallback packetCallback,
    StatusCallback statusCallback) {
  auto impl = std::make_shared<Impl>(loop, std::move(packetCallback),
                                     std::move(statusCallback));
  impl->m_exec = UvExecFunc::Create(loop);
  if (!impl->m_exec) {
    return nullptr;
  }
  impl->m_exec->wakeup.connect([](auto func) { func(); });
  MacBluetoothLEPacketClientBridge bridge;
  bridge.context = impl.get();
  bridge.setStatus = [](void* context, std::string_view status) {
    static_cast<Impl*>(context)->SetStatus(status);
  };
  bridge.setError = [](void* context, std::string_view error) {
    static_cast<Impl*>(context)->SetError(error);
  };
  bridge.setConnected = [](void* context, BluetoothPacketTransport transport) {
    static_cast<Impl*>(context)->SetConnected(transport);
  };
  bridge.setDisconnected = [](void* context, std::string_view reason) {
    static_cast<Impl*>(context)->SetDisconnected(reason);
  };
  bridge.didReceivePacket = [](void* context,
                               std::span<const uint8_t> packet) {
    static_cast<Impl*>(context)->DidReceivePacket(packet);
  };
  bridge.didSendPacket = [](void* context) {
    static_cast<Impl*>(context)->DidSendPacket();
  };
  impl->m_client =
      [[WPINetMacBluetoothLEPacketClient alloc] initWithBridge:bridge];
  return impl;
}

BluetoothLEPacketClient::Impl::Impl(uv::Loop& loop,
                                 PacketCallback packetCallback,
                                 StatusCallback statusCallback)
    : m_packetCallback{std::move(packetCallback)},
      m_statusCallback{std::move(statusCallback)} {
  (void)loop;
  m_status.supported = true;
  m_status.status = "Waiting for Bluetooth target";
}

BluetoothLEPacketClient::Impl::~Impl() {
  [m_client invalidate];
  m_client = nil;
}

bool BluetoothLEPacketClient::Impl::Connect(
    BluetoothLEPacketClientConfig config) {
  if (config.address.empty()) {
    SetError("No Bluetooth target configured");
    return false;
  }
  if (config.gattServiceUuid.empty() ||
      config.gattControlCharacteristicUuid.empty() ||
      config.gattStatusCharacteristicUuid.empty()) {
    SetError("No Bluetooth GATT UUIDs configured");
    return false;
  }

  {
    std::scoped_lock lock{m_statusMutex};
    m_config = config;
    m_status.targetAddress = config.address;
    m_status.addressType = config.addressType;
    m_status.targetConfigured = true;
    m_status.error.clear();
    m_status.status = "Connecting";
    m_status.connecting = true;
    m_status.connected = false;
  }
  PostStatus(GetStatus());

  [m_client connectWithTarget:ToNSString(config.address)
                  serviceUuid:ToNSString(config.gattServiceUuid)
                  controlUuid:ToNSString(config.gattControlCharacteristicUuid)
                   statusUuid:ToNSString(config.gattStatusCharacteristicUuid)
                maxPacketSize:config.maxPacketSize];
  return true;
}

void BluetoothLEPacketClient::Impl::Disconnect(std::string_view reason) {
  [m_client disconnectWithReason:ToNSString(reason)];
}

bool BluetoothLEPacketClient::Impl::Send(std::span<const uint8_t> packet) {
  if (packet.empty()) {
    return false;
  }

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

  NSData* data = [NSData dataWithBytes:packet.data() length:packet.size()];
  [m_client sendPacket:data];
  return true;
}

BluetoothLEPacketConnectionStatus BluetoothLEPacketClient::Impl::GetStatus() const {
  std::scoped_lock lock{m_statusMutex};
  return m_status;
}

void BluetoothLEPacketClient::Impl::SetStatus(std::string_view status) {
  UpdateStatus([&](auto& current) { current.status = status; });
}

void BluetoothLEPacketClient::Impl::SetError(std::string_view error) {
  UpdateStatus([&](auto& status) {
    status.error = error;
    status.status = error;
    status.connecting = false;
    status.connected = false;
    status.transport = BluetoothPacketTransport::NONE;
  });
}

void BluetoothLEPacketClient::Impl::SetConnected(
    BluetoothPacketTransport transport) {
  UpdateStatus([&](auto& status) {
    status.connecting = false;
    status.connected = true;
    status.transport = transport;
    status.status =
        transport == BluetoothPacketTransport::L2CAP ? "Connected (L2CAP)"
                                                      : "Connected (GATT)";
    status.error.clear();
  });
}

void BluetoothLEPacketClient::Impl::SetDisconnected(std::string_view reason) {
  UpdateStatus([&](auto& status) {
    status.connecting = false;
    status.connected = false;
    status.transport = BluetoothPacketTransport::NONE;
    status.status = reason;
  });
}

void BluetoothLEPacketClient::Impl::DidReceivePacket(
    std::span<const uint8_t> packet) {
  std::vector<uint8_t> packetCopy{packet.begin(), packet.end()};
  UpdateStatus([](auto& status) { ++status.packetsReceived; });
  if (m_packetCallback) {
    m_exec->Send([callback = m_packetCallback,
                  packetCopy = std::move(packetCopy)] {
      callback(packetCopy);
    });
  }
}

void BluetoothLEPacketClient::Impl::DidSendPacket() {
  UpdateStatus([](auto& status) { ++status.packetsSent; });
}

void BluetoothLEPacketClient::Impl::PostStatus(
    const BluetoothLEPacketConnectionStatus& status) {
  if (m_statusCallback) {
    m_exec->Send([callback = m_statusCallback, status] { callback(status); });
  }
}

std::shared_ptr<BluetoothLEPacketClient> BluetoothLEPacketClient::Create(
    wpi::net::uv::Loop& loop, PacketCallback packetCallback,
    StatusCallback statusCallback) {
  auto impl = Impl::Create(loop, std::move(packetCallback),
                           std::move(statusCallback));
  if (!impl) {
    return nullptr;
  }
  return std::shared_ptr<BluetoothLEPacketClient>(
      new BluetoothLEPacketClient{std::move(impl)});
}

BluetoothLEPacketClient::BluetoothLEPacketClient(std::shared_ptr<Impl> impl)
    : m_impl{std::move(impl)} {}

BluetoothLEPacketClient::~BluetoothLEPacketClient() = default;

bool BluetoothLEPacketClient::IsPairingSupported() {
  return false;
}

BluetoothLEDeviceScanResult BluetoothLEPacketClient::ScanDevices(
    std::chrono::milliseconds timeout) {
  WPINetMacBluetoothLEDeviceScanner* scanner =
      [[WPINetMacBluetoothLEDeviceScanner alloc] init];
  return [scanner scanForDuration:timeout.count() / 1000.0];
}

BluetoothLEPairingResult BluetoothLEPacketClient::PairDevice(
    std::string_view target) {
  (void)target;
  BluetoothLEPairingResult result;
  result.supported = false;
  result.status =
      "macOS pairs Bluetooth LE devices during connection when required";
  return result;
}

bool BluetoothLEPacketClient::Connect(BluetoothLEPacketClientConfig config) {
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
