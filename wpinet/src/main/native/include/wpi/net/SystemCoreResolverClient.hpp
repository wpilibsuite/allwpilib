// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "wpi/net/MulticastResolverClient.hpp"
#include "wpi/util/Signal.h"

namespace wpi::util {
class Logger;
}  // namespace wpi::util

namespace wpi::net {
namespace uv {
class Loop;
}  // namespace uv

class SystemCoreResolverClient
    : public std::enable_shared_from_this<SystemCoreResolverClient> {
  struct private_init {};

 public:
  struct ServerData {
    std::string serviceName;
    std::string host;
    unsigned int port = 0;
  };

  static std::shared_ptr<SystemCoreResolverClient> Create(
      wpi::net::uv::Loop& loop, wpi::util::Logger& logger, unsigned int port);
  static std::shared_ptr<SystemCoreResolverClient> Create(
      wpi::net::uv::Loop& loop, wpi::util::Logger& logger,
      std::string_view team, unsigned int port);

  SystemCoreResolverClient(wpi::net::uv::Loop& loop,
                           wpi::util::Logger& logger,
                           std::optional<std::string> team, unsigned int port,
                           const private_init&);
  ~SystemCoreResolverClient();
  SystemCoreResolverClient(const SystemCoreResolverClient&) = delete;
  SystemCoreResolverClient& operator=(const SystemCoreResolverClient&) =
      delete;

  void Close();

  wpi::util::sig::Signal<ServerData> serverResolved;

 private:
  bool Init();
  void HandleResolved(MulticastResolverClient::ServiceData data);

  wpi::net::uv::Loop& m_loop;
  wpi::util::Logger& m_logger;
  std::optional<std::string> m_team;
  unsigned int m_port;

  std::shared_ptr<MulticastResolverClient> m_resolver;
};

}  // namespace wpi::net
