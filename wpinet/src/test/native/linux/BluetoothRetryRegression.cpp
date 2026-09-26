// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "wpi/net/BluetoothLEPacketClient.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Poll.hpp"
#include "wpi/net/uv/Timer.hpp"

// Inject GATT connect errors and redirect subsequent Bluetooth connects to
// refused loopback TCP sockets. Exercise real client retries without a radio.
static sockaddr_in refusedAddress;
static int attempts;
static bool clearError;
static std::string scenario;
static int gattAttempts;
static std::chrono::steady_clock::time_point firstGatt;
static int64_t retryMs = -1;
struct BluetoothAddress {
  sa_family_t family;
  uint16_t psm;
  uint8_t address[6];
  uint16_t cid;
  uint8_t type;
};
extern "C" int getsockopt(int fd, int level, int opt, void* value,
                          socklen_t* size) {
  auto real = reinterpret_cast<int (*)(int, int, int, void*, socklen_t*)>(
      dlsym(RTLD_NEXT, "getsockopt"));
  int result = real(fd, level, opt, value, size);
  if (clearError && result == 0 && level == SOL_SOCKET && opt == SO_ERROR)
    *static_cast<int*>(value) = 0;
  return result;
}
extern "C" int socket(int domain, int type, int protocol) {
  auto real =
      reinterpret_cast<int (*)(int, int, int)>(dlsym(RTLD_NEXT, "socket"));
  return domain == AF_BLUETOOTH ? real(AF_INET, SOCK_STREAM, 0)
                                : real(domain, type, protocol);
}
extern "C" int bind(int fd, const sockaddr* addr, socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, const sockaddr*, socklen_t)>(
      dlsym(RTLD_NEXT, "bind"));
  return addr->sa_family == AF_BLUETOOTH ? 0 : real(fd, addr, size);
}
extern "C" int connect(int fd, const sockaddr* addr, socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, const sockaddr*, socklen_t)>(
      dlsym(RTLD_NEXT, "connect"));
  if (addr->sa_family == AF_BLUETOOTH) {
    ++attempts;
    if (reinterpret_cast<const BluetoothAddress*>(addr)->cid == 4) {
      ++gattAttempts;
      if (gattAttempts == 1)
        firstGatt = std::chrono::steady_clock::now();
      else
        retryMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - firstGatt)
                      .count();
      if (scenario == "fatal") {
        errno = EACCES;
        return -1;
      }
      if (gattAttempts == 1 || scenario == "persistent") {
        errno = ENOMEM;
        return -1;
      }
    }
    return real(fd, reinterpret_cast<sockaddr*>(&refusedAddress),
                sizeof(refusedAddress));
  }
  return real(fd, addr, size);
}
extern "C" int setsockopt(int fd, int level, int opt, const void* value,
                          socklen_t size) {
  auto real = reinterpret_cast<int (*)(int, int, int, const void*, socklen_t)>(
      dlsym(RTLD_NEXT, "setsockopt"));
  return level == 274 ? 0 : real(fd, level, opt, value, size);
}
int main(int argc, char** argv) {
  std::string mode = argc > 1 ? argv[1] : "fallback";
  scenario = mode;
  clearError = false;
  bool established = mode == "established";
  // Reserve an unused port without listening, guaranteeing ECONNREFUSED.
  int reserved = socket(AF_INET, SOCK_STREAM, 0);
  refusedAddress.sin_family = AF_INET;
  refusedAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(reserved, reinterpret_cast<sockaddr*>(&refusedAddress),
           sizeof(refusedAddress)))
    return 2;
  socklen_t size = sizeof(refusedAddress);
  if (getsockname(reserved, reinterpret_cast<sockaddr*>(&refusedAddress),
                  &size))
    return 2;
  if (established && listen(reserved, 1))
    return 2;
  auto loop = wpi::net::uv::Loop::Create();
  bool deadline = false;
  std::vector<std::string> errors;
  bool connectedSeen = false;
  bool retrySeen = false;
  bool replaced = false;
  bool staleSafe = true;
  std::shared_ptr<wpi::net::uv::Poll> oldPoll;
  std::shared_ptr<wpi::net::BluetoothLEPacketClient> client;
  client = wpi::net::BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) {
        if (status.status == "Retrying Bluetooth GATT connection") {
          retrySeen = true;
          if (mode == "cancel")
            client->Disconnect("Cancelled during retry");
          if (mode == "replace" && !replaced) {
            replaced = true;
            wpi::net::BluetoothLEPacketClientConfig next;
            next.address = "AA:BB:CC:DD:EE:00";
            next.preferL2CAP = false;
            next.gattServiceUuid = "00000000-0000-0000-0000-000000000001";
            next.gattControlCharacteristicUuid =
                "00000000-0000-0000-0000-000000000002";
            next.gattStatusCharacteristicUuid =
                "00000000-0000-0000-0000-000000000003";
            client->Connect(next);
          }
        }
        if (status.status == "Connecting (L2CAP)") {
          loop->Walk([&](auto& h) {
            if (auto p = dynamic_cast<wpi::net::uv::Poll*>(&h))
              oldPoll = p->shared_from_this();
          });
        }
        if (status.status == "Connecting (GATT)" && oldPoll) {
          oldPoll->error(wpi::net::uv::Error{UV_EBADF});
          staleSafe &= client->GetStatus().connecting;
          oldPoll->pollEvent(UV_READABLE | UV_WRITABLE | UV_DISCONNECT);
          staleSafe &= client->GetStatus().connecting;
        }
        if (status.connected) {
          connectedSeen = true;
          if (established) {
            int peer = accept(reserved, nullptr, nullptr);
            linger reset{1, 0};
            setsockopt(peer, SOL_SOCKET, SO_LINGER, &reset, sizeof(reset));
            close(peer);
          }
        }
        if (!status.error.empty())
          errors.push_back(status.error);
        if (mode != "cancel" && mode != "replace" && attempts &&
            !status.connecting && !status.connected && !status.error.empty())
          loop->Stop();
      });
  wpi::net::BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:FF";
  config.psm = 0x81;
  config.preferL2CAP = mode == "fallback";
  config.gattServiceUuid = "00000000-0000-0000-0000-000000000001";
  config.gattControlCharacteristicUuid = "00000000-0000-0000-0000-000000000002";
  config.gattStatusCharacteristicUuid = "00000000-0000-0000-0000-000000000003";
  client->Connect(config);
  auto timer = wpi::net::uv::Timer::Create(loop);
  timer->timeout.connect([&] {
    deadline = true;
    loop->Stop();
  });
  timer->Start(wpi::net::uv::Timer::Time{2000});
  loop->Run();
  auto status = client->GetStatus();
  std::printf("attempts=%d deadline=%d connecting=%d connected=%d error=%s\n",
              attempts, deadline, status.connecting, status.connected,
              status.error.c_str());
  bool passed =
      !status.connecting && !status.connected && staleSafe && !connectedSeen;
  if (mode == "cancel")
    passed &= deadline && attempts == 1 && retrySeen &&
              status.status == "Cancelled during retry";
  else if (mode == "replace")
    passed &= deadline && attempts == 2 && retrySeen &&
              status.targetAddress == "AA:BB:CC:DD:EE:00" &&
              status.error.find("Connection refused") != std::string::npos;
  else {
    std::string expected = mode == "persistent" ? "Cannot allocate memory"
                           : mode == "fatal"    ? "Permission denied"
                                                : "Connection refused";
    passed &= !deadline &&
              attempts == (mode == "fallback" ? 3
                           : mode == "fatal"  ? 1
                                              : 2) &&
              retrySeen == (mode != "fatal") &&
              status.error.find(expected) != std::string::npos;
    if (mode != "fatal")
      passed &= retryMs >= 200 && retryMs < 1800;
  }
  std::printf("mode=%s gatt_attempts=%d retry_ms=%" PRId64
              " retry_seen=%d stale_safe=%d "
              "passed=%d\n",
              mode.c_str(), gattAttempts, retryMs, retrySeen, staleSafe,
              passed);
  oldPoll.reset();
  client.reset();
  loop->Run(wpi::net::uv::Loop::Mode::NO_WAIT);
  loop->Walk([](wpi::net::uv::Handle& h) {
    if (!h.IsClosing())
      h.Close();
  });
  loop->Run();
  close(reserved);
  return passed ? 0 : 1;
}
