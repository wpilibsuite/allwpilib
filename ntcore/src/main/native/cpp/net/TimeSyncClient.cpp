// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TimeSyncClient.h"

#include <chrono>
#include <cstdlib>
#include <limits>
#include <mutex>

#include <wpi/net/uv/util.hpp>

#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/struct/Struct.hpp"

using namespace wpi::net;

void wpi::tsp::TimeSyncClient::UpdateStatistics(uint64_t pong_local_time,
                                                wpi::tsp::TspPing ping,
                                                wpi::tsp::TspPong pong) {
  constexpr auto MAX_TIME =
      static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
  if (pong_local_time < ping.client_time || pong_local_time > MAX_TIME ||
      ping.client_time > MAX_TIME || pong.server_time > MAX_TIME) {
    WPI_WARNING(m_logger, "Invalid TSP timestamp values");
    return;
  }
  auto rtt2 = pong_local_time - ping.client_time;
  // Compute the offset with checked timestamp ranges, without unsigned wrap.
  auto midpoint = static_cast<int64_t>(ping.client_time + rtt2 / 2);
  int64_t serverTimeOffsetUs =
      static_cast<int64_t>(pong.server_time) - midpoint;

  auto filtered = m_lastOffsets.Calculate(serverTimeOffsetUs);

  Metadata newMetadata;
  {
    std::lock_guard lock{m_metadataMutex};
    m_metadata.offset = filtered;
    m_metadata.rtt2 = rtt2;
    ++m_metadata.pongsReceived;
    m_metadata.lastPongTime = pong_local_time;
    newMetadata = m_metadata;
  }

  if (m_callback) {
    m_callback(newMetadata);
  }
}

void wpi::tsp::TimeSyncClient::Tick() {
  // Regardless of if we've gotten a pong back yet, we'll ping again. this is
  // pretty naive but should be "fine" for now?

  uint64_t ping_local_time{m_timeProvider()};

  TspPing ping{.version = 1, .message_id = 1, .client_time = ping_local_time};

  wpi::util::SmallVector<uint8_t, wpi::util::Struct<TspPing>::GetSize()>
      pingData(wpi::util::Struct<TspPing>::GetSize());
  wpi::util::PackStruct(pingData, ping);

  // Wrap our buffer - pingData should free itself
  uv::Buffer pingBuf{pingData};
  int sent = m_udp->TrySend(wpi::util::SmallVector<uv::Buffer, 1>{pingBuf});

  if (static_cast<size_t>(sent) != wpi::util::Struct<TspPing>::GetSize()) {
    WPI_ERROR(m_logger, "Didn't send the whole ping out? sent {} bytes", sent);
    return;
  }

  {
    std::lock_guard lock{m_metadataMutex};
    m_metadata.pingsSent++;
  }

  m_lastPing = ping;
}

void wpi::tsp::TimeSyncClient::UdpCallback(uv::Buffer& buf, size_t nbytes,
                                           const sockaddr& sender,
                                           unsigned flags) {
  uint64_t pong_local_time = m_timeProvider();

  if (nbytes != wpi::util::Struct<TspPong>::GetSize() ||
      (flags & UV_UDP_PARTIAL) != 0) {
    WPI_ERROR(m_logger, "Got {} bytes for pong?", nbytes);
    return;
  }

  TspPong pong{
      wpi::util::UnpackStruct<TspPong>(buf.bytes().first(nbytes)),
  };

  if (pong.version != 1) {
    WPI_WARNING(m_logger, "Bad version from server?");
    return;
  }
  if (pong.message_id != 2) {
    WPI_WARNING(m_logger, "Bad message id from server?");
    return;
  }

  TspPing ping = m_lastPing;

  if (pong.client_time != ping.client_time) {
    WPI_WARNING(m_logger,
                "Pong was not a reply to our ping? Got ping {} vs pong {}",
                ping.client_time, pong.client_time);
    return;
  }

  UpdateStatistics(pong_local_time, ping, pong);
}

wpi::tsp::TimeSyncClient::TimeSyncClient(wpi::util::Logger& logger,
                                         std::string_view server,
                                         unsigned int remote_port,
                                         std::chrono::milliseconds ping_delay,
                                         std::function<void(Metadata)> callback)
    : m_timeProvider([] { return nt::Now() / 1000; }),
      m_logger{logger},
      m_serverIP{server},
      m_serverPort{remote_port},
      m_loopDelay(ping_delay),
      m_callback(callback) {
  m_loopRunner.ExecSync([this](uv::Loop& loop) {
    struct sockaddr_in serverAddr;
    uv::NameToAddr(m_serverIP, m_serverPort, &serverAddr);

    m_udp = uv::Udp::Create(loop, AF_INET);
    m_pingTimer = uv::Timer::Create(loop);

    m_udp->Connect(serverAddr);
    m_udp->received.connect(&wpi::tsp::TimeSyncClient::UdpCallback, this);
    m_udp->StartRecv();
  });

  m_pingTimer->timeout.connect(&wpi::tsp::TimeSyncClient::Tick, this);

  m_loopRunner.ExecSync(
      [this](uv::Loop&) { m_pingTimer->Start(m_loopDelay, m_loopDelay); });
}

int64_t wpi::tsp::TimeSyncClient::GetOffset() {
  std::lock_guard lock{m_metadataMutex};
  return m_metadata.offset;
}

wpi::tsp::TimeSyncClient::Metadata wpi::tsp::TimeSyncClient::GetMetadata() {
  std::lock_guard lock{m_metadataMutex};
  return m_metadata;
}

wpi::tsp::TimeSyncClient::~TimeSyncClient() {
  m_loopRunner.Stop();
}
