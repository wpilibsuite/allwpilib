// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/FileLogger.h"

TEST(FileLoggerTest, BufferSingleLine) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("qwertyuiop\n");
  EXPECT_EQ("qwertyuiop", buf[0]);
}

TEST(FileLoggerTest, BufferMultiLine) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("line 1\nline 2\nline 3\n");
  EXPECT_EQ("line 1\nline 2\nline 3", buf[0]);
}

TEST(FileLoggerTest, BufferPartials) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  EXPECT_EQ("part 1part 2", buf[0]);
  func("\n");
  EXPECT_EQ("part 3", buf[1]);
}

TEST(FileLoggerTest, BufferMultiplePartials) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2");
  func("part 3");
  func("part 4\n");
  EXPECT_EQ("part 1part 2part 3part 4", buf[0]);
}
TEST(FileLoggerTest, BufferMultipleMultiLinePartials) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::Buffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  func("part 4\n");
  EXPECT_EQ("part 1part 2", buf[0]);
  EXPECT_EQ("part 3part 4", buf[1]);
}
