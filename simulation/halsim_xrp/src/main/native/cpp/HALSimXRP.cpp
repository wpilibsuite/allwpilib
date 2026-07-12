// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRP.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifdef __linux__
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "wpi/net/raw_uv_ostream.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/print.hpp"

namespace uv = wpi::net::uv;

using namespace wpilibxrp;

namespace {

constexpr uint16_t kXRPBluetoothPsm = 0x0081;
constexpr size_t kMaxBluetoothPacketSize = 512;

bool IsBluetoothSupported() {
#ifdef __linux__
  return true;
#else
  return false;
#endif
}

const char* AddressTypeToString(XRPBluetoothAddressType type) {
  return type == XRPBluetoothAddressType::kPublic ? "public" : "random";
}

XRPBluetoothAddressType ParseAddressType(std::string_view type) {
  if (type == "public" || type == "PUBLIC" || type == "Public") {
    return XRPBluetoothAddressType::kPublic;
  }
  return XRPBluetoothAddressType::kRandom;
}

#ifdef __linux__

#ifndef AF_BLUETOOTH
constexpr int kAfBluetooth = 31;
#else
constexpr int kAfBluetooth = AF_BLUETOOTH;
#endif
constexpr int kBluetoothProtocolL2cap = 0;
constexpr int kSolBluetooth = 274;
constexpr int kBtSecurity = 4;
constexpr uint8_t kBtSecurityLow = 1;
constexpr uint8_t kBdaddrLePublic = 0x01;
constexpr uint8_t kBdaddrLeRandom = 0x02;

#ifndef MSG_NOSIGNAL
constexpr int MSG_NOSIGNAL = 0;
#endif

struct bdaddr_t {
  uint8_t b[6];
} __attribute__((packed));

struct sockaddr_l2 {
  sa_family_t l2_family;
  uint16_t l2_psm;
  bdaddr_t l2_bdaddr;
  uint16_t l2_cid;
  uint8_t l2_bdaddr_type;
};

struct bt_security {
  uint8_t level;
  uint8_t key_size;
};

uint16_t HostToLe16(uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return value;
#else
  return static_cast<uint16_t>((value >> 8) | (value << 8));
#endif
}

int HexDigit(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  return -1;
}

bool ParseBluetoothAddress(std::string_view address, bdaddr_t* out) {
  if (address.size() != 17) {
    return false;
  }

  bdaddr_t parsed{};
  for (int i = 0; i < 6; ++i) {
    size_t pos = static_cast<size_t>(i * 3);
    if (i < 5 && address[pos + 2] != ':') {
      return false;
    }

    int high = HexDigit(address[pos]);
    int low = HexDigit(address[pos + 1]);
    if (high < 0 || low < 0) {
      return false;
    }

    // BlueZ stores Bluetooth addresses least-significant octet first.
    parsed.b[5 - i] = static_cast<uint8_t>((high << 4) | low);
  }

  *out = parsed;
  return true;
}

int SetCloseOnExecAndNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFD, 0);
  if (flags < 0 || fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
    return -1;
  }

  flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    return -1;
  }

  return 0;
}

std::string ErrnoString(std::string_view prefix) {
  std::string error{prefix};
  error.append(": ");
  error.append(std::strerror(errno));
  return error;
}

#endif

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

HALSimXRP::~HALSimXRP() {
#ifdef __linux__
  if (m_bluetoothSocket >= 0) {
    ::close(m_bluetoothSocket);
    m_bluetoothSocket = -1;
  }
#endif
}

bool HALSimXRP::Initialize() {
  if (!m_exec) {
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
    m_status.bluetoothSupported = IsBluetoothSupported();
    m_status.targetAddress = m_targetAddress;
    m_status.targetConfigured = !m_targetAddress.empty();
    m_status.addressType = m_targetAddressType;
    if (m_status.bluetoothSupported) {
      m_status.status = m_status.targetConfigured
                            ? "Ready to connect"
                            : "Waiting for Bluetooth address";
    } else {
      m_status.status = "Bluetooth transport is unsupported on this platform";
    }
  }

  wpilibxrp::WPILibUpdateFunc func = [&](const wpi::util::json& data) {
    OnNetValueChanged(data);
  };

  m_xrp.SetWPILibUpdateFunc(func);

  wpi::util::println(
      "HALSimXRP Bluetooth transport: LE L2CAP Credit-Based Mode, PSM 0x{:04x}",
      kXRPBluetoothPsm);
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

  if (!m_targetAddress.empty() && IsBluetoothSupported()) {
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
  {
    std::scoped_lock lock(m_statusMutex);
    m_targetAddress = std::move(address);
    m_targetAddressType = type;
    m_status.targetAddress = m_targetAddress;
    m_status.addressType = m_targetAddressType;
    m_status.targetConfigured = !m_targetAddress.empty();
    m_status.error.clear();
  }

  auto self = shared_from_this();
  m_exec->Send([self] { self->ConnectBluetoothOnLoop(); });
}

void HALSimXRP::DisconnectBluetooth() {
  auto self = shared_from_this();
  m_exec->Send([self] { self->CloseBluetoothOnLoop("Disconnected by user"); });
}

XRPConnectionStatus HALSimXRP::GetConnectionStatus() const {
  std::scoped_lock lock(m_statusMutex);
  return m_status;
}

void HALSimXRP::ConnectBluetoothOnLoop() {
#ifndef __linux__
  SetError("Bluetooth transport is unsupported on this platform");
#else
  if (m_targetAddress.empty()) {
    SetError("No Bluetooth address configured");
    return;
  }

  CloseBluetoothOnLoop("Connecting");

  bdaddr_t remoteAddress{};
  if (!ParseBluetoothAddress(m_targetAddress, &remoteAddress)) {
    SetError("Bluetooth address must use the form AA:BB:CC:DD:EE:FF");
    return;
  }

  int fd = ::socket(kAfBluetooth, SOCK_SEQPACKET, kBluetoothProtocolL2cap);
  if (fd < 0) {
    SetError(ErrnoString("Failed to create Bluetooth L2CAP socket"));
    return;
  }

  m_bluetoothSocket = fd;

  if (SetCloseOnExecAndNonBlock(fd) < 0) {
    SetError(ErrnoString("Failed to configure Bluetooth socket"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  bt_security security{};
  security.level = kBtSecurityLow;
  if (::setsockopt(fd, kSolBluetooth, kBtSecurity, &security,
                   sizeof(security)) < 0) {
    SetError(ErrnoString("Failed to configure Bluetooth security"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  sockaddr_l2 localAddr{};
  localAddr.l2_family = kAfBluetooth;
  localAddr.l2_bdaddr_type = kBdaddrLePublic;
  if (::bind(fd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) <
      0) {
    SetError(ErrnoString("Failed to bind Bluetooth socket"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  sockaddr_l2 remoteAddr{};
  remoteAddr.l2_family = kAfBluetooth;
  remoteAddr.l2_psm = HostToLe16(kXRPBluetoothPsm);
  remoteAddr.l2_bdaddr = remoteAddress;
  remoteAddr.l2_bdaddr_type =
      m_targetAddressType == XRPBluetoothAddressType::kPublic ? kBdaddrLePublic
                                                              : kBdaddrLeRandom;

  int result = ::connect(fd, reinterpret_cast<sockaddr*>(&remoteAddr),
                         sizeof(remoteAddr));
  if (result < 0 && errno != EINPROGRESS) {
    SetError(ErrnoString("Failed to connect Bluetooth L2CAP socket"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  m_bluetoothPoll = uv::Poll::Create(m_loop, fd);
  if (!m_bluetoothPoll) {
    SetError("Failed to create Bluetooth socket poll handle");
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  m_bluetoothPoll->pollEvent.connect([this](int events) {
    if ((events & UV_DISCONNECT) != 0) {
      CloseBluetoothOnLoop("Bluetooth connection closed");
      return;
    }
    if ((events & UV_WRITABLE) != 0) {
      CheckBluetoothConnect();
    }
    if ((events & UV_READABLE) != 0) {
      ReadBluetoothPackets();
    }
  });

  {
    std::scoped_lock lock(m_statusMutex);
    m_status.connecting = true;
    m_status.connected = false;
    m_status.status = "Connecting";
  }

  if (result == 0) {
    CheckBluetoothConnect();
  } else {
    m_bluetoothPoll->Start(UV_WRITABLE | UV_DISCONNECT);
  }
#endif
}

void HALSimXRP::CloseBluetoothOnLoop(std::string_view reason) {
#ifdef __linux__
  if (m_bluetoothPoll) {
    if (!m_bluetoothPoll->IsClosing()) {
      m_bluetoothPoll->Stop();
      m_bluetoothPoll->Close();
    }
    m_bluetoothPoll.reset();
  }

  if (m_bluetoothSocket >= 0) {
    ::close(m_bluetoothSocket);
    m_bluetoothSocket = -1;
  }
#endif

  std::scoped_lock lock(m_statusMutex);
  m_status.connecting = false;
  m_status.connected = false;
  if (!reason.empty()) {
    m_status.status = reason;
  }
}

void HALSimXRP::CheckBluetoothConnect() {
#ifdef __linux__
  if (m_bluetoothSocket < 0) {
    return;
  }

  int socketError = 0;
  socklen_t socketErrorLen = sizeof(socketError);
  if (::getsockopt(m_bluetoothSocket, SOL_SOCKET, SO_ERROR, &socketError,
                   &socketErrorLen) < 0) {
    SetError(ErrnoString("Failed to query Bluetooth connection"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  if (socketError != 0) {
    errno = socketError;
    SetError(ErrnoString("Failed to connect Bluetooth L2CAP socket"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }

  {
    std::scoped_lock lock(m_statusMutex);
    m_status.connecting = false;
    m_status.connected = true;
    m_status.status = "Connected";
    m_status.error.clear();
  }

  if (m_bluetoothPoll) {
    m_bluetoothPoll->Start(UV_READABLE | UV_DISCONNECT);
  }
#endif
}

void HALSimXRP::ReadBluetoothPackets() {
#ifdef __linux__
  if (m_bluetoothSocket < 0) {
    return;
  }

  uint8_t packet[kMaxBluetoothPacketSize];
  while (true) {
    ssize_t received = ::recv(m_bluetoothSocket, packet, sizeof(packet), 0);
    if (received > 0) {
      {
        std::scoped_lock lock(m_statusMutex);
        ++m_status.packetsReceived;
      }
      ParsePacket({packet, static_cast<size_t>(received)});
      continue;
    }

    if (received == 0) {
      CloseBluetoothOnLoop("Bluetooth connection closed");
      return;
    }

    if (errno == EINTR) {
      continue;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return;
    }

    SetError(ErrnoString("Bluetooth receive failed"));
    CloseBluetoothOnLoop("Disconnected");
    return;
  }
#endif
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
  static uv::SimpleBufferPool<4> bufferPool(kMaxBluetoothPacketSize);
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
#ifdef __linux__
  bool connected = false;
  {
    std::scoped_lock lock(m_statusMutex);
    connected = m_status.connected;
  }

  if (connected && m_bluetoothSocket >= 0) {
    size_t packetSize = 0;
    for (const auto& buf : sendBufs) {
      packetSize += buf.len;
    }

    if (packetSize <= kMaxBluetoothPacketSize) {
      std::vector<uint8_t> packet;
      packet.reserve(packetSize);
      for (const auto& buf : sendBufs) {
        const auto* data = reinterpret_cast<const uint8_t*>(buf.base);
        packet.insert(packet.end(), data, data + buf.len);
      }

      ssize_t sent =
          ::send(m_bluetoothSocket, packet.data(), packet.size(), MSG_NOSIGNAL);
      if (sent == static_cast<ssize_t>(packet.size())) {
        std::scoped_lock lock(m_statusMutex);
        ++m_status.packetsSent;
      } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK &&
                 errno != EINTR) {
        SetError(ErrnoString("Bluetooth send failed"));
        CloseBluetoothOnLoop("Disconnected");
      }
    } else {
      SetError("XRP packet is larger than Bluetooth transport MTU");
    }
  }
#endif

  std::lock_guard lock(m_buffer_mutex);
  GetBufferPool().Release(sendBufs);
}

void HALSimXRP::SetStatus(std::string_view status) {
  std::scoped_lock lock(m_statusMutex);
  m_status.status = status;
}

void HALSimXRP::SetError(std::string_view error) {
  std::scoped_lock lock(m_statusMutex);
  m_status.error = error;
  m_status.status = error;
  m_status.connecting = false;
  m_status.connected = false;
}
