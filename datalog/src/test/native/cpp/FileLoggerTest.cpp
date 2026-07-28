// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/FileLogger.hpp"

#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("FileLoggerTest BufferSingleLine", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("qwertyuiop\n");
  CHECK("qwertyuiop" == buf[0]);
}

TEST_CASE("FileLoggerTest BufferMultiLine", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("line 1\nline 2\nline 3\n");
  CHECK("line 1\nline 2\nline 3" == buf[0]);
}

TEST_CASE("FileLoggerTest BufferPartials", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  CHECK("part 1part 2" == buf[0]);
  func("\n");
  CHECK("part 3" == buf[1]);
}

TEST_CASE("FileLoggerTest BufferMultiplePartials", "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2");
  func("part 3");
  func("part 4\n");
  CHECK("part 1part 2part 3part 4" == buf[0]);
}
TEST_CASE("FileLoggerTest BufferMultipleMultiLinePartials",
          "[datalog][file-logger]") {
  std::vector<std::string> buf;
  auto func = wpi::log::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  func("part 4\n");
  CHECK("part 1part 2" == buf[0]);
  CHECK("part 3part 4" == buf[1]);
}
