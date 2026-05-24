// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TimeSyncServer.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>

#include "wpi/net/UDPClient.hpp"
#include "wpi/net/uv/util.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/struct/Struct.hpp"

using namespace wpi::net;

void wpi::tsp::TimeSyncServer::UdpCallback(uv::Buffer& data, size_t n,
                                           const sockaddr& sender,
                                           unsigned flags) {
  TspPing ping{wpi::util::UnpackStruct<TspPing>(data.bytes())};

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

  wpi::util::SmallVector<uint8_t, wpi::util::Struct<TspPong>::GetSize()>
      pongData(wpi::util::Struct<TspPong>::GetSize());
  wpi::util::PackStruct(pongData, pong);

  // Wrap our buffer - pongData should free itself for free
  uv::Buffer pongBuf{pongData};
  int sent =
      m_udp->TrySend(sender, wpi::util::SmallVector<uv::Buffer, 1>{pongBuf});

  if (static_cast<size_t>(sent) != wpi::util::Struct<TspPong>::GetSize()) {
    WPI_ERROR(m_logger, "Didn't send the whole pong back?");
    return;
  }
}

wpi::tsp::TimeSyncServer::TimeSyncServer(wpi::util::Logger& logger,
                                         std::string_view listenAddress,
                                         unsigned int port)
    : m_timeProvider{nt::Now}, m_logger{logger}, m_udp{} {
  m_loopRunner.ExecSync([this, listenAddress, port](uv::Loop& loop) {
    m_udp = {uv::Udp::Create(loop, AF_INET)};
    m_udp->Bind(listenAddress, port);
    m_udp->received.connect(&wpi::tsp::TimeSyncServer::UdpCallback, this);
    m_udp->StartRecv();
  });
}
