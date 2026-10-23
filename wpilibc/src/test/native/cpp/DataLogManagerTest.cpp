// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/DataLogManager.hpp"

#include <chrono>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/simulation/AlertSim.hpp"
#include "wpi/util/Alert.hpp"
#include "wpi/util/fs.hpp"

TEST_CASE("DataLogManagerTest MatchLogFilenameDetection", "[wpilibc]") {
  for (std::string_view stem : {
           "WPILIB_20261022_123456_CALA_P1.wpilog",
           "WPILIB_20261022_123456_WAMIL_Q123.wpilog",
           "WPILIB_20261022_123456_MNDU2_E13.wpilog",
           "WPILIB_20261022_123456_Event_T4.wpilog",
       }) {
    CAPTURE(stem);
    CHECK(wpi::internal::IsDataLogMatchLog(stem));
  }

  for (std::string_view stem : {
           "WPILIB_20261022_123456",
           "WPILIB_20261022_123456_Event_Q",
           "WPILIB_20261022_123456_Event_Qabc",
           "WPILIB_20261022_123456_Event_X3",
           "WPILIB_TBD_1234567890abcdef",
       }) {
    CAPTURE(stem);
    CHECK_FALSE(wpi::internal::IsDataLogMatchLog(stem));
  }
}

TEST_CASE("DataLogManagerTest FreeSpaceAlert", "[wpilibc]") {
  HAL_Initialize();
  wpi::util::Alert alert{"DataLogManagerTest", "lowSpace", "",
                         wpi::util::Alert::Level::MEDIUM};

  // Include both sides of the warning and cleanup thresholds, then recover.
  for (uintmax_t freeSpace : {500000000, 400000000, 399999999, 200000000,
                              199999999, 0, 100000000, 400000000}) {
    CAPTURE(freeSpace);
    wpi::internal::UpdateDataLogFreeSpaceAlert(alert, freeSpace);
    CHECK(alert.Get() == (freeSpace < 400000000));
    if (alert.Get()) {
      CHECK(alert.GetText().contains(std::to_string(freeSpace / 1000000) +
                                     " MB of free space remaining"));
      CHECK(alert.GetText().contains("at startup below 200 MB"));
    }
  }
}

TEST_CASE("DataLogManagerTest AlertLifetime", "[wpilibc]") {
  HAL_Initialize();
  auto logDir =
      fs::temp_directory_path() /
      ("DataLogManagerTest_" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directory(logDir);
  struct Cleanup {
    fs::path dir;
    bool running = false;
    ~Cleanup() {
      if (running) {
        wpi::DataLogManager::Stop();
      }
      std::error_code ec;
      fs::remove_all(dir, ec);
    }
  } cleanup{logDir};

  for (int i = 0; i < 2; ++i) {
    wpi::DataLogManager::Start(logDir.string());
    cleanup.running = true;
    int count = 0;
    for (const auto& alert : wpi::sim::AlertSim::GetAll()) {
      if (alert.group != "DataLogManager") {
        continue;
      }
      ++count;
      if (alert.id == "internalStorage") {
        CHECK_FALSE(alert.isActive());
        CHECK(alert.level == wpi::util::Alert::Level::LOW);
      } else {
        CHECK(alert.id == "lowSpace");
        CHECK(alert.level == wpi::util::Alert::Level::MEDIUM);
      }
    }
    CHECK(count == 2);
    wpi::DataLogManager::Stop();
    cleanup.running = false;
    for (const auto& alert : wpi::sim::AlertSim::GetAll()) {
      CHECK(alert.group != "DataLogManager");
    }
  }
}
