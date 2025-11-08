// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/net/MulticastServiceAnnouncer.h"
#include "wpi/net/MulticastServiceResolver.h"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/UidVector.hpp"

namespace wpi {
struct MulticastHandleManager {
  wpi::mutex mutex;
  wpi::UidVector<int, 8> handleIds;
  wpi::DenseMap<size_t, std::unique_ptr<wpi::MulticastServiceResolver>>
      resolvers;
  wpi::DenseMap<size_t, std::unique_ptr<wpi::MulticastServiceAnnouncer>>
      announcers;
#ifdef _WIN32
  ~MulticastHandleManager();
#endif
};

MulticastHandleManager& GetMulticastManager();
}  // namespace wpi
