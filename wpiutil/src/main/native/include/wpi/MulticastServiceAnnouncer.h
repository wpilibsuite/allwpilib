// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/span.h"

namespace wpi {
class MulticastServiceAnnouncer {
 public:
  MulticastServiceAnnouncer(std::string_view serviceName, std::string_view serviceType,
                int port,
                wpi::span<const std::pair<std::string, std::string>> txt);
  ~MulticastServiceAnnouncer() noexcept;

  void Start();
  void Stop();

  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};
}  // namespace wpi
