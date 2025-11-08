// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/net/MulticastServiceAnnouncer.h"
#include "wpi/net/MulticastServiceResolver.h"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/UidVector.hpp"

namespace wpi::net {
struct MulticastHandleManager {
  wpi::util::mutex mutex;
  wpi::util::UidVector<int, 8> handleIds;
  wpi::util::DenseMap<size_t,
                      std::unique_ptr<wpi::net::MulticastServiceResolver>>
      resolvers;
  wpi::util::DenseMap<size_t,
                      std::unique_ptr<wpi::net::MulticastServiceAnnouncer>>
      announcers;
#ifdef _WIN32
  ~MulticastHandleManager();
#endif
};

MulticastHandleManager& GetMulticastManager();
}  // namespace wpi::net
