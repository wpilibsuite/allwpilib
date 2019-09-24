/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogFile.h"  // NOLINT(build/include_order)

#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>

#include <wpi/FileSystem.h>

#include "gtest/gtest.h"

std::string GetExpectedFilename(std::string filenamePrefix) {
  std::time_t m_time = std::time(0);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d-%H_%M_%S",
                std::localtime(&m_time));
  return filenamePrefix + "-" + datetime + ".txt";
}

TEST(LogFileTest, Logs) {
  std::string filename;

  {
    frc::LogFile logFile("testLog");

    logFile.Log("First line\n");
    logFile.Logln("Second line");
    logFile << "Line " << 3;

    filename = logFile.GetFileName();
  }

  std::ifstream testFile(filename);

  std::string line;
  std::getline(testFile, line);
  EXPECT_EQ("First line", line);

  std::getline(testFile, line);
  EXPECT_EQ("Second line", line);

  std::getline(testFile, line);
  EXPECT_EQ("Line 3", line);

  testFile.close();

  std::remove(filename.c_str());
}

TEST(LogFileTest, Filename) {
  std::string filename;

  {
    std::string filenamePrefix = "testFilename";

    frc::LogFile logFile(filenamePrefix);
    logFile.SetTimeIntervalBeforeRenaming(0.9_s);

    std::string firstFilename = GetExpectedFilename(filenamePrefix);
    EXPECT_TRUE(wpi::sys::fs::exists(firstFilename));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    logFile.Log("");

    filename = GetExpectedFilename(filenamePrefix);
    EXPECT_FALSE(wpi::sys::fs::exists(firstFilename));
    EXPECT_TRUE(wpi::sys::fs::exists(filename));
  }

  std::remove(filename.c_str());
}
