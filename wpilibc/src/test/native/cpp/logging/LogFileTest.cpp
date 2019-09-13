/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogFile.h"  // NOLINT(build/include_order)

#include <fstream>
#include <string>

#include "gtest/gtest.h"

TEST(LogFileTest, WriteLog) {
  frc::LogFile* logFile = new frc::LogFile("logfile");

  logFile->Log("First line\n");
  logFile->Logln("Second line");
  *logFile << "Line " << 3;

  std::string filename = logFile->GetFileName();
  delete logFile;

  std::ifstream testFile(filename);

  std::string line;
  std::getline(testFile, line);
  EXPECT_EQ("First line", line);

  std::getline(testFile, line);
  EXPECT_EQ("Second line", line);

  std::getline(testFile, line);
  EXPECT_EQ("Line 3", line);
}
