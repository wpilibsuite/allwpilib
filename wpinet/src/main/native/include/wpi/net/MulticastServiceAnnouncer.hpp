// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/net/MulticastServiceAnnouncer.h"

namespace wpi::net {

class MulticastServiceAnnouncer {
 public:
  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param txt txt
   */
  MulticastServiceAnnouncer(
      std::string_view serviceName, std::string_view serviceType, int port,
      std::span<const std::pair<std::string, std::string>> txt);

  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param txt txt
   */
  MulticastServiceAnnouncer(
      std::string_view serviceName, std::string_view serviceType, int port,
      std::span<const std::pair<std::string_view, std::string_view>> txt);

  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   */
  MulticastServiceAnnouncer(std::string_view serviceName,
                            std::string_view serviceType, int port);

  ~MulticastServiceAnnouncer() noexcept;

  /**
   * Starts multicast service announcer.
   */
  void Start();

  /**
   * Stops multicast service announcer.
   */
  void Stop();

  /**
   * Returns true if there's a multicast service announcer implementation.
   *
   * @return True if there's a multicast service announcer implementation.
   */
  bool HasImplementation() const;

  struct Impl;

 private:
  std::unique_ptr<Impl> pImpl;
};

}  // namespace wpi::net
