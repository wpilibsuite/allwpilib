// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <string_view>

#include "wpi/net/MulticastServiceResolver.h"
#include "wpi/util/Signal.h"

namespace wpi::util {
class Logger;
}  // namespace wpi::util

namespace wpi::net {
namespace uv {
class Loop;
template <typename... T>
class Async;
class Timer;
}  // namespace uv

class MulticastResolverClient
    : public std::enable_shared_from_this<MulticastResolverClient> {
  struct private_init {};

 public:
  using ServiceData = MulticastServiceResolver::ServiceData;

  static std::shared_ptr<MulticastResolverClient> Create(
      wpi::net::uv::Loop& loop, wpi::util::Logger& logger,
      std::string_view serviceType);

  MulticastResolverClient(wpi::net::uv::Loop& loop,
                          wpi::util::Logger& logger,
                          std::string_view serviceType, const private_init&);
  ~MulticastResolverClient();
  MulticastResolverClient(const MulticastResolverClient&) = delete;
  MulticastResolverClient& operator=(const MulticastResolverClient&) = delete;

  void Close();

  wpi::util::sig::Signal<ServiceData> serviceResolved;

 private:
  struct ResolverData {
    uint64_t generation = 0;
    ServiceData data;
  };

  void Init();
  void Restart();
  void Stop();
  void HandleResolved(ResolverData data);

  wpi::net::uv::Loop& m_loop;
  wpi::util::Logger& m_logger;
  std::string m_serviceType;

  std::unique_ptr<MulticastServiceResolver> m_resolver;
  std::shared_ptr<wpi::net::uv::Async<ResolverData>> m_resolverData;
  std::shared_ptr<wpi::net::uv::Timer> m_timer;
  uint64_t m_generation = 0;
  bool m_warnedNoImplementation = false;
};

}  // namespace wpi::net
