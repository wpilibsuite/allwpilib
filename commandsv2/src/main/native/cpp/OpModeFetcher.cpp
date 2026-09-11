// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/OpModeFetcher.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "wpi/driverstation/RobotState.hpp"

using namespace wpi::cmd;

std::unique_ptr<OpModeFetcher> OpModeFetcher::s_fetcher{nullptr};

OpModeFetcher& OpModeFetcher::GetFetcher() {
  // Default to pull from the DS unless otherwise specified
  if (!s_fetcher) {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (!s_fetcher) {
      s_fetcher = std::make_unique<DriverStationOpModeFetcher>();
    }
  }

  return *s_fetcher;
}

void OpModeFetcher::SetFetcher(std::unique_ptr<OpModeFetcher> fetcher) {
  s_fetcher = std::move(fetcher);
}

int64_t DriverStationOpModeFetcher::GetOpModeId() {
  return wpi::RobotState::GetOpModeId();
}

std::string DriverStationOpModeFetcher::GetOpModeName() {
  return wpi::RobotState::GetOpMode();
}
