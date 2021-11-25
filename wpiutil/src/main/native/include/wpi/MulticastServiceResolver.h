// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/span.h"

namespace wpi {
class MulticastServiceResolver {
 public:
  using mDnsRevolveCompletion =
      void(unsigned int ipv4Address, int port, std::string_view serviceName,
           std::string_view hostName,
           wpi::span<const std::pair<std::string, std::string>> txt);
  using mDnsRevolveCompletionFunc = std::function<mDnsRevolveCompletion>;

  MulticastServiceResolver(std::string_view serviceType, mDnsRevolveCompletionFunc onFound);
  ~MulticastServiceResolver() noexcept;

  void Start();
  void Stop();

  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
