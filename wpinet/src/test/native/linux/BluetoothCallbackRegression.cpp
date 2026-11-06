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
#include <thread>
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
  uint8_t addressLastByte = 0;
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
static bool blockSend;

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
  connections.back().addressLastByte =
      reinterpret_cast<const BluetoothAddress*>(address)->address[0];
  if (scenario.starts_with("send-") || scenario.starts_with("destroy-") ||
      scenario.starts_with("overtake-") || scenario.starts_with("invalid-") ||
      scenario.starts_with("receive-") || scenario.starts_with("disconnect-")) {
    return 0;
  }
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
  if (blockSend) {
    blockSend = false;
    errno = EAGAIN;
    return -1;
  }
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

static int RunSendRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  auto client = BluetoothLEPacketClient::Create(*loop, [](auto) {});
  auto timer = uv::Timer::Create(loop);
  bool oldAccepted = false;
  bool newAccepted = false;
  bool full = false;
  timer->timeout.connect([&] {
    BluetoothLEPacketClientConfig config;
    config.address = "AA:BB:CC:DD:EE:01";
    config.psm = 0x81;
    client->Connect(config);
    const uint8_t oldPacket[] = {0x12, 0x34};
    if (scenario == "send-blocked") {
      blockSend = true;
      oldAccepted = client->Send(oldPacket, BluetoothPacketSendMode::QUEUED);
    } else {
      // Keep the loop busy while the old send waits in its async queue.
      std::thread worker{[&] {
        oldAccepted =
            client->Send(oldPacket, scenario == "send-queued"
                                        ? BluetoothPacketSendMode::QUEUED
                                        : BluetoothPacketSendMode::BEST_EFFORT);
      }};
      worker.join();
    }

    config.address = "AA:BB:CC:DD:EE:02";
    client->Connect(config);
    const uint8_t newPacket[] = {0x56, 0x78};
    std::thread worker{[&] {
      newAccepted = client->Send(newPacket, BluetoothPacketSendMode::QUEUED);
      full = !client->Send(newPacket, BluetoothPacketSendMode::QUEUED);
    }};
    worker.join();
    timer->Close();
  });
  timer->Start(uv::Timer::Time{0});
  auto deadline = uv::Timer::Create(loop);
  deadline->timeout.connect([&] { loop->Stop(); });
  deadline->Start(uv::Timer::Time{50});
  loop->Run();

  bool passed = oldAccepted && newAccepted && full && connections.size() == 2 &&
                client->GetStatus().connected;
  if (connections.size() == 2) {
    uint8_t packet[32];
    passed &= recv(connections[0].peer, packet, sizeof(packet), 0) == 0;
    auto received = recv(connections[1].peer, packet, sizeof(packet), 0);
    passed &= received == 2 && packet[0] == 0x56 && packet[1] == 0x78;
    received = recv(connections[1].peer, packet, sizeof(packet), 0);
    passed &= received < 0 && (errno == EAGAIN || errno == EWOULDBLOCK);
    // Draining the current send must also release its queue reservation.
    const uint8_t finalPacket[] = {0x9a};
    passed &= client->Send(finalPacket, BluetoothPacketSendMode::QUEUED);
    loop->Run(uv::Loop::Mode::NO_WAIT);
    passed &= recv(connections[1].peer, packet, sizeof(packet), 0) == 1 &&
              packet[0] == 0x9a;
  }
  std::printf("%s: old_accepted=%d new_accepted=%d full=%d passed=%d\n",
              scenario.c_str(), oldAccepted, newAccepted, full, passed);
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

static int RunReceiveRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  std::vector<uint8_t> received;
  int callbacks = 0;
  auto client = BluetoothLEPacketClient::Create(*loop, [&](auto packet) {
    ++callbacks;
    received.assign(packet.begin(), packet.end());
  });
  BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:01";
  config.psm = 0x81;
  config.maxPacketSize = 4;
  client->Connect(config);
  loop->Run(uv::Loop::Mode::NO_WAIT);
  bool passed = client->GetStatus().connected && connections.size() == 1;
  bool oversized = scenario == "receive-oversize";
  const uint8_t packet[] = {1, 2, 3, 4, 5, 6};
  send(connections.back().peer, packet, oversized ? 6 : 4, MSG_NOSIGNAL);
  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }
  auto status = client->GetStatus();
  if (oversized) {
    passed &= callbacks == 0 && status.packetsReceived == 0 &&
              !status.connected && !status.error.empty();
  } else {
    passed &= callbacks == 1 && status.packetsReceived == 1 &&
              status.connected &&
              received == std::vector<uint8_t>(packet, packet + 4);
  }
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
  std::printf("%s: passed=%d\n", scenario.c_str(), passed);
  return passed ? 0 : 1;
}

static int RunEmptyDisconnectRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  std::vector<BluetoothLEPacketConnectionStatus> statuses;
  auto client = BluetoothLEPacketClient::Create(
      *loop, [](auto) {},
      [&](const auto& status) { statuses.push_back(status); });
  BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:01";
  config.psm = 0x81;
  auto starter = uv::Timer::Create(loop);
  bool passed = true;
  starter->timeout.connect([&] {
    client->Connect(config);
    passed &= client->GetStatus().connected && connections.size() == 1;
    statuses.clear();
    if (scenario == "disconnect-empty-thread") {
      std::thread worker{[&] { client->Disconnect(""); }};
      worker.join();
    } else {
      client->Disconnect("");
    }
    starter->Close();
  });
  starter->Start(uv::Timer::Time{0});
  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }
  auto status = client->GetStatus();
  const uint8_t packet[] = {0x12};
  passed &= !status.connected && !status.connecting && status.status.empty() &&
            status.transport == BluetoothPacketTransport::NONE &&
            !client->Send(packet) && statuses.size() == 1 &&
            statuses.back().status.empty() && !statuses.back().connected;
  uint8_t received[32];
  passed &= recv(connections.back().peer, received, sizeof(received), 0) == 0;
  statuses.clear();
  client.reset();
  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }
  passed &= statuses.empty() && !loop->IsAlive();
  loop->Walk([](auto& handle) {
    if (!handle.IsClosing()) {
      handle.Close();
    }
  });
  loop->Run();
  for (auto& connection : connections) {
    close(connection.peer);
  }
  std::printf("%s: passed=%d\n", scenario.c_str(), passed);
  return passed ? 0 : 1;
}

static int RunRejectedConfigRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  std::vector<uint8_t> received;
  auto client = BluetoothLEPacketClient::Create(*loop, [&](auto packet) {
    received.assign(packet.begin(), packet.end());
  });
  auto timer = uv::Timer::Create(loop);
  bool passed = true;
  const uint8_t packet[] = {0x12, 0x34};
  timer->timeout.connect([&] {
    BluetoothLEPacketClientConfig config;
    config.address = "AA:BB:CC:DD:EE:01";
    config.psm = 0x81;
    client->Connect(config);
    auto reject = [&] {
      passed &= !client->Connect({});
      config.psm = 0;
      passed &= !client->Connect(config);
    };
    if (scenario == "invalid-config-thread") {
      std::thread worker{reject};
      worker.join();
    } else {
      reject();
    }
    auto status = client->GetStatus();
    passed &= status.connected && !status.connecting &&
              status.transport == BluetoothPacketTransport::L2CAP &&
              status.targetAddress == config.address && !status.error.empty();
    passed &= client->Send(packet);
    send(connections.back().peer, packet, sizeof(packet), MSG_NOSIGNAL);
    timer->Close();
  });
  timer->Start(uv::Timer::Time{0});
  auto deadline = uv::Timer::Create(loop);
  deadline->timeout.connect([&] { loop->Stop(); });
  deadline->Start(uv::Timer::Time{50});
  loop->Run();
  passed &=
      connections.size() == 1 && client->GetStatus().connected &&
      received == std::vector<uint8_t>(std::begin(packet), std::end(packet));
  uint8_t sent[2];
  passed &= recv(connections.back().peer, sent, sizeof(sent), 0) == 2 &&
            std::equal(std::begin(packet), std::end(packet), sent);
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
  std::printf("%s: passed=%d\n", scenario.c_str(), passed);
  return passed ? 0 : 1;
}

static int RunRequestOrderRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  auto client = BluetoothLEPacketClient::Create(*loop, [](auto) {});
  auto timer = uv::Timer::Create(loop);
  bool cancel = scenario == "overtake-connect-cancel";
  bool disconnect = scenario == "overtake-disconnect";
  timer->timeout.connect([&] {
    BluetoothLEPacketClientConfig config;
    config.address = "AA:BB:CC:DD:EE:01";
    config.psm = 0x81;
    if (disconnect) {
      client->Connect(config);
    }
    // The caller queues its request while the loop is still in this callback.
    std::thread worker{[&] {
      if (disconnect) {
        client->Disconnect("Old disconnect");
      } else {
        client->Connect(config);
      }
    }};
    worker.join();
    if (cancel) {
      client->Disconnect("Cancelled");
    } else {
      config.address = "AA:BB:CC:DD:EE:02";
      client->Connect(config);
    }
    timer->Close();
  });
  timer->Start(uv::Timer::Time{0});
  for (int i = 0; i < 3; ++i) {
    loop->Run(uv::Loop::Mode::NO_WAIT);
  }
  auto status = client->GetStatus();
  bool passed;
  if (cancel) {
    passed = connections.empty() && !status.connecting && !status.connected &&
             status.status == "Cancelled";
  } else {
    passed = connections.size() == (disconnect ? 2u : 1u) &&
             connections.back().addressLastByte == 2 && status.connected &&
             status.targetAddress == "AA:BB:CC:DD:EE:02";
    const uint8_t packet[] = {0x12, 0x34};
    passed &= client->Send(packet);
    loop->Run(uv::Loop::Mode::NO_WAIT);
    uint8_t received[2];
    passed &=
        recv(connections.back().peer, received, sizeof(received), 0) == 2 &&
        std::equal(std::begin(packet), std::end(packet), received);
  }
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
  std::printf("%s: passed=%d\n", scenario.c_str(), passed);
  return passed ? 0 : 1;
}

static int RunTeardownRegression() {
  using namespace wpi::net;
  auto loop = uv::Loop::Create();
  auto client = BluetoothLEPacketClient::Create(*loop, [](auto) {});
  BluetoothLEPacketClientConfig config;
  config.address = "AA:BB:CC:DD:EE:01";
  config.psm = 0x81;
  client->Connect(config);
  loop->Run(uv::Loop::Mode::NO_WAIT);
  bool passed = client->GetStatus().connected && connections.size() == 1;

  if (scenario == "destroy-after-loop") {
    loop->SetClosing();
    loop->Walk([](auto& handle) { handle.Close(); });
    loop->Run();
    loop.reset();
    client.reset();
  } else {
    client.reset();
    for (int i = 0; i < 3; ++i) {
      loop->Run(uv::Loop::Mode::NO_WAIT);
    }
    passed &= !loop->IsAlive();
    int handles = 0;
    loop->Walk([&](auto& handle) {
      ++handles;
      if (!handle.IsClosing()) {
        handle.Close();
      }
    });
    passed &= handles == 0;
    loop->Run();
  }
  if (connections.size() == 1) {
    uint8_t packet[32];
    // The native connection must close even if the loop closes first.
    passed &= recv(connections[0].peer, packet, sizeof(packet), 0) == 0;
  }
  for (auto& connection : connections) {
    close(connection.peer);
  }
  std::printf("%s: passed=%d\n", scenario.c_str(), passed);
  return passed ? 0 : 1;
}

int main(int argc, char** argv) {
  using namespace wpi::net;
  scenario = argc > 1 ? argv[1] : "cancel-l2cap";
  if (scenario.starts_with("receive-")) {
    return RunReceiveRegression();
  }
  if (scenario.starts_with("disconnect-")) {
    return RunEmptyDisconnectRegression();
  }
  if (scenario.starts_with("invalid-")) {
    return RunRejectedConfigRegression();
  }
  if (scenario.starts_with("overtake-")) {
    return RunRequestOrderRegression();
  }
  if (scenario.starts_with("send-")) {
    return RunSendRegression();
  }
  if (scenario.starts_with("destroy-")) {
    return RunTeardownRegression();
  }
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
