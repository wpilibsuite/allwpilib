// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/BluetoothL2CAPClient.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Poll.hpp"

namespace uv = wpi::net::uv;

using namespace wpi::net;

namespace {

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

  ~Impl() { CloseOnLoop({}); }

  bool Connect(BluetoothL2CAPClientConfig config) {
    if (config.address.empty()) {
      SetError("No Bluetooth address configured");
      return false;
    }
    if (config.psm == 0) {
      SetError("No Bluetooth L2CAP PSM configured");
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
    PublishStatus();

    auto self = shared_from_this();
    m_exec->Send(
        [self, config = std::move(config)] { self->ConnectOnLoop(config); });
    return true;
  }

  void Disconnect(std::string_view reason) {
    auto self = shared_from_this();
    std::string reasonString{reason};
    m_exec->Send([self, reasonString = std::move(reasonString)] {
      self->CloseOnLoop(reasonString);
    });
  }

  bool Send(std::span<const uint8_t> packet) {
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

    std::vector<uint8_t> packetCopy{packet.begin(), packet.end()};
    auto self = shared_from_this();
    m_exec->Send([self, packetCopy = std::move(packetCopy)] {
      self->SendOnLoop(packetCopy);
    });
    return true;
  }

  BluetoothL2CAPConnectionStatus GetStatus() const {
    std::scoped_lock lock{m_statusMutex};
    return m_status;
  }

 private:
  void ConnectOnLoop(const BluetoothL2CAPClientConfig& config) {
    CloseOnLoop("Connecting");

    bdaddr_t remoteAddress{};
    if (!ParseBluetoothAddress(config.address, &remoteAddress)) {
      SetError("Bluetooth address must use the form AA:BB:CC:DD:EE:FF");
      return;
    }

    int fd = ::socket(kAfBluetooth, SOCK_SEQPACKET, kBluetoothProtocolL2cap);
    if (fd < 0) {
      SetError(ErrnoString("Failed to create Bluetooth L2CAP socket"));
      return;
    }

    m_socket = fd;

    if (SetCloseOnExecAndNonBlock(fd) < 0) {
      SetError(ErrnoString("Failed to configure Bluetooth socket"));
      CloseOnLoop("Disconnected");
      return;
    }

    bt_security security{};
    security.level = kBtSecurityLow;
    if (::setsockopt(fd, kSolBluetooth, kBtSecurity, &security,
                     sizeof(security)) < 0) {
      SetError(ErrnoString("Failed to configure Bluetooth security"));
      CloseOnLoop("Disconnected");
      return;
    }

    sockaddr_l2 localAddr{};
    localAddr.l2_family = kAfBluetooth;
    localAddr.l2_bdaddr_type = kBdaddrLePublic;
    if (::bind(fd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) <
        0) {
      SetError(ErrnoString("Failed to bind Bluetooth socket"));
      CloseOnLoop("Disconnected");
      return;
    }

    sockaddr_l2 remoteAddr{};
    remoteAddr.l2_family = kAfBluetooth;
    remoteAddr.l2_psm = HostToLe16(config.psm);
    remoteAddr.l2_bdaddr = remoteAddress;
    remoteAddr.l2_bdaddr_type =
        config.addressType == BluetoothAddressType::kPublic ? kBdaddrLePublic
                                                            : kBdaddrLeRandom;

    int result = ::connect(fd, reinterpret_cast<sockaddr*>(&remoteAddr),
                           sizeof(remoteAddr));
    if (result < 0 && errno != EINPROGRESS) {
      SetError(ErrnoString("Failed to connect Bluetooth L2CAP socket"));
      CloseOnLoop("Disconnected");
      return;
    }

    m_poll = uv::Poll::Create(m_loop, fd);
    if (!m_poll) {
      SetError("Failed to create Bluetooth socket poll handle");
      CloseOnLoop("Disconnected");
      return;
    }

    auto self = shared_from_this();
    m_poll->pollEvent.connect([self](int events) {
      if ((events & UV_DISCONNECT) != 0) {
        self->CloseOnLoop("Bluetooth connection closed");
        return;
      }
      if ((events & UV_WRITABLE) != 0) {
        self->CheckConnect();
      }
      if ((events & UV_READABLE) != 0) {
        self->ReadPackets();
      }
    });

    SetConnecting();

    if (result == 0) {
      CheckConnect();
    } else {
      m_poll->Start(UV_WRITABLE | UV_DISCONNECT);
    }
  }

  void CloseOnLoop(std::string_view reason) {
    if (m_poll) {
      if (!m_poll->IsClosing()) {
        m_poll->Stop();
        m_poll->Close();
      }
      m_poll.reset();
    }

    if (m_socket >= 0) {
      ::close(m_socket);
      m_socket = -1;
    }

    if (!reason.empty()) {
      UpdateStatus([&](auto& status) {
        status.connecting = false;
        status.connected = false;
        status.status = reason;
      });
    }
  }

  void CheckConnect() {
    if (m_socket < 0) {
      return;
    }

    int socketError = 0;
    socklen_t socketErrorLen = sizeof(socketError);
    if (::getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &socketError,
                     &socketErrorLen) < 0) {
      SetError(ErrnoString("Failed to query Bluetooth connection"));
      CloseOnLoop("Disconnected");
      return;
    }

    if (socketError != 0) {
      errno = socketError;
      SetError(ErrnoString("Failed to connect Bluetooth L2CAP socket"));
      CloseOnLoop("Disconnected");
      return;
    }

    UpdateStatus([](auto& status) {
      status.connecting = false;
      status.connected = true;
      status.status = "Connected";
      status.error.clear();
    });

    if (m_poll) {
      m_poll->Start(UV_READABLE | UV_DISCONNECT);
    }
  }

  void ReadPackets() {
    if (m_socket < 0) {
      return;
    }

    std::vector<uint8_t> packet(m_config.maxPacketSize);
    while (true) {
      ssize_t received = ::recv(m_socket, packet.data(), packet.size(), 0);
      if (received > 0) {
        UpdateStatus([](auto& status) { ++status.packetsReceived; });
        m_packetCallback({packet.data(), static_cast<size_t>(received)});
        continue;
      }

      if (received == 0) {
        CloseOnLoop("Bluetooth connection closed");
        return;
      }

      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }

      SetError(ErrnoString("Bluetooth receive failed"));
      CloseOnLoop("Disconnected");
      return;
    }
  }

  void SendOnLoop(std::span<const uint8_t> packet) {
    if (m_socket < 0) {
      return;
    }

    ssize_t sent = ::send(m_socket, packet.data(), packet.size(), MSG_NOSIGNAL);
    if (sent == static_cast<ssize_t>(packet.size())) {
      UpdateStatus([](auto& status) { ++status.packetsSent; });
    } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK &&
               errno != EINTR) {
      SetError(ErrnoString("Bluetooth send failed"));
      CloseOnLoop("Disconnected");
    }
  }

  void SetConnecting() {
    UpdateStatus([](auto& status) {
      status.connecting = true;
      status.connected = false;
      status.status = "Connecting";
    });
  }

  void SetError(std::string_view error) {
    UpdateStatus([&](auto& status) {
      status.error = error;
      status.status = error;
      status.connecting = false;
      status.connected = false;
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

  void PublishStatusSnapshot(const BluetoothL2CAPConnectionStatus& snapshot) {
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
    PublishStatusSnapshot(snapshot);
  }

  uv::Loop& m_loop;
  PacketCallback m_packetCallback;
  StatusCallback m_statusCallback;
  std::shared_ptr<UvExecFunc> m_exec;

  mutable std::mutex m_statusMutex;
  BluetoothL2CAPConnectionStatus m_status;
  BluetoothL2CAPClientConfig m_config;

  std::shared_ptr<uv::Poll> m_poll;
  int m_socket = -1;
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
