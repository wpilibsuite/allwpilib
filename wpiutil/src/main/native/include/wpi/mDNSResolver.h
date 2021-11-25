// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/span.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace wpi {
class mDNSResolver {
 public:
  typedef void mDnsRevolveCompletion(
      unsigned int ipv4Address, std::string_view serviceName,
      std::string_view hostName,
      wpi::span<std::pair<std::string, std::string>> txt);
  typedef mDnsRevolveCompletion* mDnsRevolveCompletionFunc;

  mDNSResolver(std::string_view serviceType, mDnsRevolveCompletionFunc onFound);
  ~mDNSResolver() noexcept;

  void Start();
  void Stop();

  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
