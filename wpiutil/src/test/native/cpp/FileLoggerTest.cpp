// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/FileLogger.h"

TEST(FileLoggerTest, LineBufferSingleLine) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::LineBuffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("qwertyuiop\n");
  EXPECT_EQ(buf.front(), "qwertyuiop");
  buf.clear();
}

TEST(FileLoggerTest, LineBufferMultiLine) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::LineBuffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("line 1\nline 2\nline 3\n");
  EXPECT_EQ("line 1", buf[0]);
  EXPECT_EQ("line 2", buf[1]);
  EXPECT_EQ("line 3", buf[2]);
}

TEST(FileLoggerTest, LineBufferPartials) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::LineBuffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2\npart 3");
  EXPECT_EQ("part 1part 2", buf.front());
  buf.clear();
  func("\n");
  EXPECT_EQ("part 3", buf.front());
}

TEST(FileLoggerTest, LineBufferMultiplePartials) {
  std::vector<std::string> buf;
  auto func = wpi::FileLogger::LineBuffer(
      [&buf](std::string_view line) { buf.emplace_back(line); });
  func("part 1");
  func("part 2");
  func("part 3");
  func("part 4\n");
  EXPECT_EQ("part 1part 2part 3part 4", buf.front());
}
