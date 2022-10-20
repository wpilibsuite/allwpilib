// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ntcore_cpp.h"

namespace nt {

class INetworkClient {
 public:
  virtual ~INetworkClient() = default;

  virtual void SetServers(
      std::span<const std::pair<std::string, unsigned int>> servers) = 0;

  virtual void StartDSClient(unsigned int port) = 0;
  virtual void StopDSClient() = 0;

  virtual void FlushLocal() = 0;
  virtual void Flush() = 0;
};

}  // namespace nt
