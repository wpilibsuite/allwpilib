// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MulticastHandleManager.hpp"

using namespace wpi;

MulticastHandleManager& wpi::GetMulticastManager() {
  static MulticastHandleManager manager;
  return manager;
}

#ifdef _WIN32
MulticastHandleManager::~MulticastHandleManager() {
  // Multicast handles cannot be safely destructed on windows during shutdown.
  // Just leak all handles.
  for (auto&& i : resolvers) {
    i.second.release();
  }

  for (auto&& i : announcers) {
    i.second.release();
  }
}
#endif
