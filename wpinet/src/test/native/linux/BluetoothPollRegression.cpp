// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "wpi/net/BluetoothLEPacketClient.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Poll.hpp"
#include "wpi/net/uv/Timer.hpp"

// Redirect only Bluetooth operations to refused loopback TCP connections. This
// exercises the real libuv POLLERR -> UV_EBADF path without touching the radio.
static sockaddr_in refusedAddress;
static int attempts;
static bool clearError;
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
  clearError = mode == "cleared";
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
  bool staleSafe = true;
  std::shared_ptr<wpi::net::uv::Poll> oldPoll;
  std::shared_ptr<wpi::net::BluetoothLEPacketClient> client;
  client = wpi::net::BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) {
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
        if (attempts && !status.connecting && !status.connected &&
            !status.error.empty())
          loop->Stop();
      });
  wpi::net::BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:FF";
  config.psm = 0x81;
  config.preferL2CAP = mode != "gatt";
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
  std::string expected = established  ? "Connection reset by peer"
                         : clearError ? "bad file descriptor"
                                      : "Connection refused";
  bool passed = attempts == ((established || mode == "gatt") ? 1 : 2) &&
                !deadline && !status.connecting && !status.connected &&
                status.error.find(expected) != std::string::npos &&
                connectedSeen == established && staleSafe;
  std::printf("mode=%s stale_safe=%d connected_seen=%d passed=%d\n",
              mode.c_str(), staleSafe, connectedSeen, passed);
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
