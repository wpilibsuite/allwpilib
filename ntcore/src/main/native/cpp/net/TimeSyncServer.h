// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdlib>
#include <functional>
#include <memory>

#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/UDPClient.hpp"
#include "wpi/net/uv/Buffer.hpp"
#include "wpi/net/uv/Udp.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/util/Logger.hpp"

namespace wpi::tsp {

class TimeSyncServer {
  using SharedUdpPtr = std::shared_ptr<wpi::net::uv::Udp>;

  std::function<uint64_t()> m_timeProvider;
  wpi::util::Logger& m_logger;
  wpi::net::EventLoopRunner m_loopRunner;
  SharedUdpPtr m_udp;

 public:
  // Create, bind, and start listening for TSP pings
  TimeSyncServer(wpi::util::Logger& logger, std::string_view listenAddress,
                 unsigned int port = NT_DEFAULT_PORT);

  // Stop our internal loop runner and unbind
  ~TimeSyncServer() = default;

 private:
  void UdpCallback(wpi::net::uv::Buffer& buf, size_t nbytes,
                   const sockaddr& sender, unsigned flags);
};

}  // namespace wpi::tsp
