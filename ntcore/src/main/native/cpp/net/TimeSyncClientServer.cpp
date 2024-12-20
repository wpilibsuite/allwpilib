// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/TimeSyncClientServer.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>

#include <wpi/Logger.h>
#include <wpi/print.h>
#include <wpi/struct/Struct.h>
#include <wpinet/UDPClient.h>
#include <wpinet/uv/util.h>

#include "ntcore_cpp.h"

template <>
struct wpi::Struct<TspPing> {
  static constexpr std::string_view GetTypeName() { return "TspPing"; }
  static constexpr size_t GetSize() { return 10; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time";
  }

  static TspPing Unpack(std::span<const uint8_t> data) {
    return TspPing{
        wpi::UnpackStruct<uint8_t, 0>(data),
        wpi::UnpackStruct<uint8_t, 1>(data),
        wpi::UnpackStruct<uint64_t, 2>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const TspPing& value) {
    wpi::PackStruct<0>(data, value.version);
    wpi::PackStruct<1>(data, value.message_id);
    wpi::PackStruct<2>(data, value.client_time);
  }
};

template <>
struct wpi::Struct<TspPong> {
  static constexpr std::string_view GetTypeName() { return "TspPong"; }
  static constexpr size_t GetSize() { return 18; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time;uint64_t "
           "server_time";
  }

  static TspPong Unpack(std::span<const uint8_t> data) {
    return TspPong{
        TspPing{
            wpi::UnpackStruct<uint8_t, 0>(data),
            wpi::UnpackStruct<uint8_t, 1>(data),
            wpi::UnpackStruct<uint64_t, 2>(data),
        },
        wpi::UnpackStruct<uint64_t, 10>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const TspPong& value) {
    wpi::PackStruct<0>(data, value.version);
    wpi::PackStruct<1>(data, value.message_id);
    wpi::PackStruct<2>(data, value.client_time);
    wpi::PackStruct<10>(data, value.server_time);
  }
};

static_assert(wpi::StructSerializable<TspPong>);
static_assert(wpi::StructSerializable<TspPing>);

static void ClientLoggerFunc(unsigned int level, const char* file,
                             unsigned int line, const char* msg) {
  if (level == 20) {
    wpi::print(stderr, "TimeSyncClient: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "TimeSyncClient: {}: {} ({}:{})\n", levelmsg, msg, file,
             line);
}

static void ServerLoggerFunc(unsigned int level, const char* file,
                             unsigned int line, const char* msg) {
  if (level == 20) {
    wpi::print(stderr, "TimeSyncServer: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "TimeSyncServer: {}: {} ({}:{})\n", levelmsg, msg, file,
             line);
}

void wpi::TimeSyncServer::UdpCallback(uv::Buffer& data, size_t n,
                                      const sockaddr& sender, unsigned flags) {
  wpi::println("TimeSyncServer got ping!");

  TspPing ping{wpi::UnpackStruct<TspPing>(data.bytes())};

  if (ping.version != 1) {
    WPI_ERROR(m_logger, "Bad version from client?");
    return;
  }
  if (ping.message_id != 1) {
    WPI_ERROR(m_logger, "Bad message id from client?");
    return;
  }

  uint64_t current_time = m_timeProvider();

  TspPong pong{ping, current_time};
  pong.message_id = 2;

  wpi::SmallVector<uint8_t, wpi::Struct<TspPong>::GetSize()> pongData(
      wpi::Struct<TspPong>::GetSize());
  wpi::PackStruct(pongData, pong);

  // Wrap our buffer - pongData should free itself for free
  wpi::uv::Buffer pongBuf{pongData};
  int sent =
      m_udp->TrySend(sender, wpi::SmallVector<wpi::uv::Buffer, 1>{pongBuf});
  wpi::println("Pong ret: {}", sent);
  if (static_cast<size_t>(sent) != wpi::Struct<TspPong>::GetSize()) {
    WPI_ERROR(m_logger, "Didn't send the whole pong back?");
    return;
  }

  WPI_INFO(m_logger, "Got ping: {} {} {}", ping.version, ping.message_id,
           ping.client_time);
  WPI_INFO(m_logger, "Sent pong: {} {} {} {}", pong.version, pong.message_id,
           pong.client_time, pong.server_time);
}

wpi::TimeSyncServer::TimeSyncServer(int port,
                                    std::function<uint64_t()> timeProvider)
    : m_logger{::ServerLoggerFunc},
      m_timeProvider{timeProvider},
      m_udp{wpi::uv::Udp::Create(m_loopRunner.GetLoop(), AF_INET)} {
  m_loopRunner.ExecSync(
      [this, port](uv::Loop&) { m_udp->Bind("0.0.0.0", port); });
}

void wpi::TimeSyncServer::Start() {
  m_loopRunner.ExecSync([this](uv::Loop&) {
    m_udp->received.connect(&wpi::TimeSyncServer::UdpCallback, this);
    m_udp->StartRecv();
  });
}

void wpi::TimeSyncServer::Stop() {
  m_loopRunner.Stop();
}

void wpi::TimeSyncClient::Tick() {
  wpi::println("wpi::TimeSyncClient::Tick");
  // Regardless of if we've gotten a pong back yet, we'll ping again. this is
  // pretty naive but should be "fine" for now?

  uint64_t ping_local_time{m_timeProvider()};

  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_local_time};

  wpi::SmallVector<uint8_t, wpi::Struct<TspPing>::GetSize()> pingData(
      wpi::Struct<TspPing>::GetSize());
  wpi::PackStruct(pingData, ping);

  // Wrap our buffer - pingData should free itself
  wpi::uv::Buffer pingBuf{pingData};
  int sent = m_udp->TrySend(wpi::SmallVector<wpi::uv::Buffer, 1>{pingBuf});

  if (static_cast<size_t>(sent) != wpi::Struct<TspPing>::GetSize()) {
    WPI_ERROR(m_logger, "Didn't send the whole ping out?");
    return;
  }

  {
    std::lock_guard lock{m_offsetMutex};
    m_metadata.pingsSent++;
  }

  m_lastPing = ping;
}

void wpi::TimeSyncClient::UdpCallback(uv::Buffer& buf, size_t nbytes,
                                      const sockaddr& sender, unsigned flags) {
  uint64_t pong_local_time = m_timeProvider();

  if (static_cast<size_t>(nbytes) != wpi::Struct<TspPong>::GetSize()) {
    WPI_ERROR(m_logger, "Got {} bytes for pong?", nbytes);
    return;
  }

  TspPong pong{
      wpi::UnpackStruct<TspPong>(buf.bytes()),
  };

  fmt::println("->[client] Got pong: {} {} {} {}", pong.version,
               pong.message_id, pong.client_time, pong.server_time);

  if (pong.version != 1) {
    fmt::println("Bad version from server?");
    return;
  }
  if (pong.message_id != 2) {
    fmt::println("Bad message id from server?");
    return;
  }

  TspPing ping = m_lastPing;

  if (pong.client_time != ping.client_time) {
    WPI_WARNING(m_logger,
                "Pong was not a reply to our ping? Got ping {} vs pong {}",
                ping.client_time, pong.client_time);
    return;
  }

  // when time = send_time+rtt2/2, server time = server time
  // server time = local time + offset
  // offset = (server time - local time) = (server time) - (send_time +
  // rtt2/2)
  auto rtt2 = pong_local_time - ping.client_time;
  int64_t serverTimeOffsetUs = pong.server_time - rtt2 / 2 - ping.client_time;

  {
    std::lock_guard lock{m_offsetMutex};
    m_metadata.offset = serverTimeOffsetUs;
    m_metadata.pongsRecieved++;
    m_metadata.lastPongTime = pong_local_time;
  }

  using std::cout;
  fmt::println("Ping-ponged! RTT2 {} uS, offset {} uS", rtt2,
               serverTimeOffsetUs);
  fmt::println("Estimated server time {} s",
               (m_timeProvider() + serverTimeOffsetUs) / 1000000.0);
}

wpi::TimeSyncClient::TimeSyncClient(std::string_view server, int remote_port,
                                    std::chrono::milliseconds ping_delay,
                                    std::function<uint64_t()> timeProvider)
    : m_logger(::ClientLoggerFunc),
      m_timeProvider(timeProvider),
      m_udp{wpi::uv::Udp::Create(m_loopRunner.GetLoop(), AF_INET)},
      m_pingTimer{wpi::uv::Timer::Create(m_loopRunner.GetLoop())},
      m_serverIP{server},
      m_serverPort{remote_port},
      m_loopDelay(ping_delay) {
  struct sockaddr_in serverAddr;
  uv::NameToAddr(m_serverIP, m_serverPort, &serverAddr);

  m_loopRunner.ExecSync(
      [this, serverAddr](uv::Loop&) { m_udp->Connect(serverAddr); });
}

void wpi::TimeSyncClient::Start() {
  wpi::println("Connecting recieved");

  m_loopRunner.ExecSync([this](uv::Loop&) {
    m_udp->received.connect(&wpi::TimeSyncClient::UdpCallback, this);
    m_udp->StartRecv();
  });

  wpi::println("Starting pinger");
  using namespace std::chrono_literals;
  m_pingTimer->timeout.connect(&wpi::TimeSyncClient::Tick, this);

  m_loopRunner.ExecSync(
      [this](uv::Loop&) { m_pingTimer->Start(m_loopDelay, m_loopDelay); });
}

void wpi::TimeSyncClient::Stop() {
  m_loopRunner.Stop();
}

int64_t wpi::TimeSyncClient::GetOffset() {
  std::lock_guard lock{m_offsetMutex};
  return m_metadata.offset;
}

wpi::TimeSyncClient::Metadata wpi::TimeSyncClient::GetMetadata() {
  std::lock_guard lock{m_offsetMutex};
  return m_metadata;
}
