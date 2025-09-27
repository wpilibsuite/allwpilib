// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/util/DenseMap.h>
#include <wpi/util/UidVector.hpp>

#include "wpi/net/MulticastServiceAnnouncer.hpp"
#include "wpi/net/MulticastServiceResolver.hpp"

namespace wpi::net {
struct MulticastHandleManager {
  wpi::util::mutex mutex;
  wpi::util::UidVector<int, 8> handleIds;
  wpi::util::DenseMap<size_t, std::unique_ptr<wpi::net::MulticastServiceResolver>>
      resolvers;
  wpi::util::DenseMap<size_t, std::unique_ptr<wpi::net::MulticastServiceAnnouncer>>
      announcers;
#ifdef _WIN32
  ~MulticastHandleManager();
#endif
};

MulticastHandleManager& GetMulticastManager();
}  // namespace wpi::net
