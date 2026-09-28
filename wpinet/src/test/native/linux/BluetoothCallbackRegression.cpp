// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <dlfcn.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "wpi/net/BluetoothLEPacketClient.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Timer.hpp"

// Use connected packet sockets and a minimal ATT server to cancel or replace
// connections from each progress callback without Bluetooth hardware.
struct Connection {
  int socket;
  int peer;
  bool gatt = false;
};

struct BluetoothAddress {
  sa_family_t family;
  uint16_t psm;
  uint8_t address[6];
  uint16_t cid;
  uint8_t type;
};

static std::vector<Connection> connections;
static std::string scenario;

extern "C" int socket(int domain, int type, int protocol) {
  auto real =
      reinterpret_cast<int (*)(int, int, int)>(dlsym(RTLD_NEXT, "socket"));
  if (domain != AF_BLUETOOTH) {
    return real(domain, type, protocol);
  }
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_NONBLOCK, 0, sockets) < 0) {
    return -1;
  }
  connections.push_back({sockets[0], sockets[1]});
  return sockets[0];
}

extern "C" int bind(int fd, const sockaddr* address, socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, const sockaddr*, socklen_t)>(
      dlsym(RTLD_NEXT, "bind"));
  return address->sa_family == AF_BLUETOOTH ? 0 : real(fd, address, size);
}

extern "C" int connect(int fd, const sockaddr* address, socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, const sockaddr*, socklen_t)>(
      dlsym(RTLD_NEXT, "connect"));
  if (address->sa_family != AF_BLUETOOTH) {
    return real(fd, address, size);
  }
  connections.back().gatt =
      reinterpret_cast<const BluetoothAddress*>(address)->cid == 4;
  errno = scenario.ends_with("fallback") && connections.size() == 1
              ? EPROTONOSUPPORT
              : EINPROGRESS;
  return -1;
}

extern "C" int setsockopt(int fd, int level, int option, const void* value,
                          socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, int, int, const void*, socklen_t)>(
      dlsym(RTLD_NEXT, "setsockopt"));
  return level == 274 ? 0 : real(fd, level, option, value, size);
}

extern "C" ssize_t send(int fd, const void* data, size_t size, int flags) {
  auto real = reinterpret_cast<ssize_t (*)(int, const void*, size_t, int)>(
      dlsym(RTLD_NEXT, "send"));
  auto connection = std::find_if(connections.rbegin(), connections.rend(),
                                 [=](auto& c) { return c.socket == fd; });
  ssize_t result = real(fd, data, size, flags);
  if (connection == connections.rend() || !connection->gatt || result <= 0) {
    return result;
  }

  auto pdu = std::span{static_cast<const uint8_t*>(data), size};
  std::vector<uint8_t> response;
  switch (pdu[0]) {
    case 0x02:  // Exchange MTU
      response = scenario.ends_with("error")
                     ? std::vector<uint8_t>{0x03}
                     : std::vector<uint8_t>{0x03, 0x05, 0x02};
      break;
    case 0x06:  // Find service, handles 1 through 6
      response = {0x07, 1, 0, 6, 0};
      break;
    case 0x08:  // Read characteristic declarations
      if (pdu[1] > 4) {
        response = {0x01, 0x08, pdu[1], 0, 0x0a};
      } else {
        response = {0x09, 21};
        for (uint8_t uuid : {2, 3}) {
          uint8_t handle = 2 * (uuid - 1);
          response.insert(
              response.end(),
              {handle, 0, 0x14, static_cast<uint8_t>(handle + 1), 0, uuid});
          response.insert(response.end(), 15, 0);
        }
      }
      break;
    case 0x04:  // Notification descriptor at handle 6
      response = {0x05, 1, 6, 0, 0x02, 0x29};
      break;
    case 0x12:  // Enable notifications
      response = {0x13};
      break;
    default:
      return result;
  }
  real(connection->peer, response.data(), response.size(), MSG_NOSIGNAL);
  return result;
}

int main(int argc, char** argv) {
  using namespace wpi::net;
  scenario = argc > 1 ? argv[1] : "cancel-l2cap";
  bool replace = scenario.starts_with("replace-");
  std::string stage = scenario.substr(scenario.find('-') + 1);
  std::string trigger;
  if (stage == "l2cap") {
    trigger = "Connecting (L2CAP)";
  } else if (stage == "gatt") {
    trigger = "Connecting (GATT)";
  } else if (stage == "fallback") {
    trigger = "L2CAP unavailable; connecting GATT";
  } else if (stage == "mtu") {
    trigger = "Negotiating Bluetooth GATT MTU";
  } else if (stage == "service") {
    trigger = "Discovering Bluetooth GATT service";
  } else if (stage == "characteristics") {
    trigger = "Discovering Bluetooth GATT characteristics";
  } else if (stage == "descriptor") {
    trigger = "Discovering Bluetooth GATT notification descriptor";
  } else if (stage == "notifications") {
    trigger = "Enabling Bluetooth GATT notifications";
  } else if (stage == "error") {
    trigger = "Malformed Bluetooth GATT MTU response";
  } else {
    return 2;
  }

  auto loop = uv::Loop::Create();
  bool acted = false;
  bool lateStatus = false;
  std::shared_ptr<BluetoothLEPacketClient> client;
  client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) {
        if (acted) {
          lateStatus |= replace ? status.targetAddress != "AA:BB:CC:DD:EE:02"
                                : status.status != "Cancelled";
          return;
        }
        if (status.status != trigger) {
          return;
        }
        acted = true;
        if (replace) {
          BluetoothLEPacketClientConfig next;
          next.address = "AA:BB:CC:DD:EE:02";
          next.psm = 0x81;
          client->Connect(next);
        } else {
          client->Disconnect("Cancelled");
        }
      });
  BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:01";
  config.psm = 0x81;
  config.preferL2CAP = stage == "l2cap" || stage == "fallback";
  config.gattServiceUuid = "00000000-0000-0000-0000-000000000001";
  config.gattControlCharacteristicUuid = "00000000-0000-0000-0000-000000000002";
  config.gattStatusCharacteristicUuid = "00000000-0000-0000-0000-000000000003";
  client->Connect(config);

  auto timer = uv::Timer::Create(loop);
  timer->timeout.connect([&] { loop->Stop(); });
  timer->Start(uv::Timer::Time{50});
  loop->Run();
  auto status = client->GetStatus();
  bool passed = acted && !lateStatus && !status.connecting &&
                connections.size() == (replace ? 2u : 1u);
  if (replace) {
    uint8_t packet[32];
    errno = 0;
    auto received = recv(connections.back().peer, packet, sizeof(packet), 0);
    passed &= status.connected &&
              status.transport == BluetoothPacketTransport::L2CAP &&
              received < 0 && (errno == EAGAIN || errno == EWOULDBLOCK);
  } else {
    passed &= !status.connected && status.status == "Cancelled";
  }
  std::printf("%s: acted=%d late_status=%d connections=%zu passed=%d\n",
              scenario.c_str(), acted, lateStatus, connections.size(), passed);
  client.reset();
  loop->Run(uv::Loop::Mode::NO_WAIT);
  loop->Walk([](auto& handle) {
    if (!handle.IsClosing()) {
      handle.Close();
    }
  });
  loop->Run();
  for (auto& connection : connections) {
    close(connection.peer);
  }
  return passed ? 0 : 1;
}
