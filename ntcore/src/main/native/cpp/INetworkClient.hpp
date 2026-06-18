// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <utility>

#include "wpi/nt/ntcore_cpp.hpp"

namespace wpi::nt {

class INetworkClient {
 public:
  struct ServerResolver {
    std::string serviceType;
    std::string serviceName;
    bool serviceNamePrefix = false;
    bool requireTeam = false;
    std::string team;
    unsigned int port = 0;
    bool useResolvedPort = false;
  };

  virtual ~INetworkClient() = default;

  virtual void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) = 0;
  virtual void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers,
      const ServerResolver& resolver) = 0;
  virtual void Disconnect() = 0;

  virtual void StartDSClient(unsigned int port) = 0;
  virtual void StopDSClient() = 0;

  virtual void FlushLocal() = 0;
  virtual void Flush() = 0;
};

}  // namespace wpi::nt
