/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogFile.h"  // NOLINT(build/include_order)

#include <ctime>
#include <fstream>
#include <string>

#include <wpi/FileSystem.h>

#include "gtest/gtest.h"

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
}

TEST(LogFileTest, Filename) {
  std::string filenamePrefix = "testFilename";
  frc::LogFile logFile(filenamePrefix);

  std::time_t m_time = std::time(0);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d-%H_%M_%S",
                std::localtime(&m_time));

  EXPECT_TRUE(wpi::sys::fs::exists(filenamePrefix + "-" + datetime + ".txt"));
}
