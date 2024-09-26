// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/MulticastServiceAnnouncer.h"

#include <memory>
#include <utility>

#include <wpi/SmallVector.h>

#include "MulticastHandleManager.h"

extern "C" {
WPI_MulticastServiceAnnouncerHandle WPI_CreateMulticastServiceAnnouncer(
    const char* serviceName, const char* serviceType, int32_t port,
    int32_t txtCount, const char** keys, const char** values)

{
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};

  wpi::SmallVector<std::pair<std::string_view, std::string_view>, 8> txts;

  for (int32_t i = 0; i < txtCount; i++) {
    txts.emplace_back(
        std::pair<std::string_view, std::string_view>{keys[i], values[i]});
  }

  auto announcer = std::make_unique<wpi::MulticastServiceAnnouncer>(
      serviceName, serviceType, port, txts);

  size_t index = manager.handleIds.emplace_back(3);
  manager.announcers[index] = std::move(announcer);

  return index;
}

void WPI_FreeMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  manager.announcers[handle] = nullptr;
  manager.handleIds.erase(handle);
}

void WPI_StartMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Start();
}

void WPI_StopMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  announcer->Stop();
}

int32_t WPI_GetMulticastServiceAnnouncerHasImplementation(
    WPI_MulticastServiceAnnouncerHandle handle) {
  auto& manager = wpi::GetMulticastManager();
  std::scoped_lock lock{manager.mutex};
  auto& announcer = manager.announcers[handle];
  return announcer->HasImplementation();
}
}  // extern "C"
