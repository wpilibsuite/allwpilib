// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/DataLogManager.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

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
           "WPILIB_20261022_123456_CALA_P1.2.wpilog",
           "WPILIB_20261022_123456_WAMIL_Q123.12.wpilog",
           "WPILIB_20261022_123456_MNDU2_E13.3.wpilog",
           "WPILIB_20261022_123456_Event_T4.2.wpilog",
       }) {
    CAPTURE(stem);
    CHECK(wpi::internal::IsDataLogMatchLog(stem));
  }

  for (std::string_view stem : {
           "WPILIB_20261022_123456.wpilog",
           "WPILIB_20261022_123456_Event_Q.wpilog",
           "WPILIB_20261022_123456_Event_Qabc.wpilog",
           "WPILIB_20261022_123456_Event_X3.wpilog",
           "WPILIB_TBD_1234567890abcdef.wpilog",
           "WPILIB_20261022_123456_Event_Q3.wpilog.bak",
           "WPILIB_20261022_123456_Event_Q.2.wpilog",
           "WPILIB_20261022_123456_Event_Q1..2.wpilog",
           "WPILIB_20261022_123456_Event_Q1.2.3.wpilog",
           "WPILIB_20261022_123456_Event_Q1.abc.wpilog",
           "WPILIB_20261022_123456_Event_Q1..wpilog",
           "WPILIB_20261022_123456.2.wpilog",
       }) {
    CAPTURE(stem);
    CHECK_FALSE(wpi::internal::IsDataLogMatchLog(stem));
  }
}

TEST_CASE("DataLogManagerTest Cleanup", "[wpilibc]") {
  HAL_Initialize();
  auto logDir =
      fs::temp_directory_path() /
      ("DataLogManagerCleanupTest_" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directory(logDir);
  struct Cleanup {
    fs::path dir;
    ~Cleanup() {
      std::error_code ec;
      fs::remove_all(dir, ec);
    }
  } cleanup{logDir};

  int matchCount = 20;
  int nonMatchCount = 10;
  int deletedCount = 10;
  uintmax_t freeSpace = 0;
  SECTION("Protected logs count toward the minimum") {}
  SECTION("Retain ten permanent logs in total") {
    matchCount = 5;
    deletedCount = 5;
  }
  SECTION("Stop once sufficient space is recovered") {
    freeSpace = 199999950;
    deletedCount = 1;
  }
  SECTION("Do not delete logs when sufficient space is available") {
    freeSpace = 200000000;
    deletedCount = 0;
  }
  SECTION("Temporary and unrelated files do not count toward the minimum") {
    matchCount = 9;
    nonMatchCount = 1;
    deletedCount = 0;
  }

  auto timestamp = fs::file_time_type::clock::now();
  auto createLog = [&](std::string_view filename) {
    auto path = logDir / filename;
    {
      std::ofstream{path} << std::string(100, 'x');
    }
    REQUIRE(fs::file_size(path) == 100);
    fs::last_write_time(path, timestamp);
    timestamp += std::chrono::seconds{1};
    return path;
  };
  std::vector<fs::path> matches;
  for (int i = 0; i < matchCount; ++i) {
    matches.emplace_back(createLog(std::format(
        "WPILIB_20261022_123456_Event_Q{}{}.wpilog", i, i % 2 ? ".2" : "")));
  }
  std::vector<fs::path> nonMatches;
  for (int i = 0; i < nonMatchCount; ++i) {
    nonMatches.emplace_back(
        createLog(std::format("WPILIB_20261022_1234{:02}.wpilog", i)));
  }
  auto temporary = createLog("WPILIB_TBD_1234567890abcdef.wpilog");
  auto unrelated = createLog("unrelated.wpilog");

  wpi::internal::DeleteOldDataLogs(logDir.string(), freeSpace);

  for (const auto& path : matches) {
    CAPTURE(path);
    CHECK(fs::exists(path));
  }
  for (int i = 0; i < nonMatchCount; ++i) {
    CAPTURE(i);
    CHECK(fs::exists(nonMatches[i]) == (i >= deletedCount));
  }
  CHECK(fs::exists(temporary));
  CHECK(fs::exists(unrelated));
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
