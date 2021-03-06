// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Preferences.h"  // NOLINT(build/include_order)

#include <cstdio>
#include <fstream>

#include <networktables/NetworkTableInstance.h>
#include <ntcore.h>
#include <units/time.h>

#include "frc/Timer.h"
#include "gtest/gtest.h"

static const char* kFileName = "networktables.ini";
static constexpr auto kSaveTime = 1.2_s;

/**
 * If we write a new networktables.ini with some sample values, test that
 * we get those same values back using the Preference class.
 */
TEST(PreferencesTest, ReadPreferencesFromFile) {
  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.StopServer();

  std::remove(kFileName);
  std::ofstream preferencesFile(kFileName);
  preferencesFile << "[NetworkTables Storage 3.0]" << std::endl;
  preferencesFile
      << "string \"/Preferences/testFileGetString\"=\"Hello, preferences file\""
      << std::endl;
  preferencesFile << "double \"/Preferences/testFileGetInt\"=1" << std::endl;
  preferencesFile << "double \"/Preferences/testFileGetDouble\"=0.5"
                  << std::endl;
  preferencesFile << "double \"/Preferences/testFileGetFloat\"=0.25"
                  << std::endl;
  preferencesFile << "boolean \"/Preferences/testFileGetBoolean\"=true"
                  << std::endl;
  preferencesFile
      << "double \"/Preferences/testFileGetLong\"=1000000000000000000"
      << std::endl;
  preferencesFile.close();

  inst.StartServer();

  EXPECT_EQ("Hello, preferences file",
            frc::Preferences::GetString("testFileGetString"));
  EXPECT_EQ(1, frc::Preferences::GetInt("testFileGetInt"));
  EXPECT_FLOAT_EQ(0.5, frc::Preferences::GetDouble("testFileGetDouble"));
  EXPECT_FLOAT_EQ(0.25f, frc::Preferences::GetFloat("testFileGetFloat"));
  EXPECT_TRUE(frc::Preferences::GetBoolean("testFileGetBoolean"));
  EXPECT_EQ(1000000000000000000ll,
            frc::Preferences::GetLong("testFileGetLong"));
}

/**
 * If we set some values using the Preferences class, test that they show up
 * in networktables.ini
 */
TEST(PreferencesTest, WritePreferencesToFile) {
  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.StartServer();
  frc::Preferences::Remove("testFileGetString");
  frc::Preferences::Remove("testFileGetInt");
  frc::Preferences::Remove("testFileGetDouble");
  frc::Preferences::Remove("testFileGetFloat");
  frc::Preferences::Remove("testFileGetBoolean");
  frc::Preferences::Remove("testFileGetLong");

  frc::Wait(kSaveTime);

  frc::Preferences::SetString("testFileSetString", "Hello, preferences file");
  frc::Preferences::SetInt("testFileSetInt", 1);
  frc::Preferences::SetDouble("testFileSetDouble", 0.5);
  frc::Preferences::SetFloat("testFileSetFloat", 0.25f);
  frc::Preferences::SetBoolean("testFileSetBoolean", true);
  frc::Preferences::SetLong("testFileSetLong", 1000000000000000000ll);

  frc::Wait(kSaveTime);

  static char const* kExpectedFileContents[] = {
      "[NetworkTables Storage 3.0]",
      "string \"/Preferences/.type\"=\"RobotPreferences\"",
      "boolean \"/Preferences/testFileSetBoolean\"=true",
      "double \"/Preferences/testFileSetDouble\"=0.5",
      "double \"/Preferences/testFileSetFloat\"=0.25",
      "double \"/Preferences/testFileSetInt\"=1",
      "double \"/Preferences/testFileSetLong\"=1e+18",
      "string \"/Preferences/testFileSetString\"=\"Hello, preferences file\""};

  std::ifstream preferencesFile(kFileName);
  for (auto& kExpectedFileContent : kExpectedFileContents) {
    ASSERT_FALSE(preferencesFile.eof())
        << "Preferences file prematurely reached EOF";

    std::string line;
    std::getline(preferencesFile, line);

    ASSERT_EQ(kExpectedFileContent, line)
        << "A line in networktables.ini was not correct";
  }
}
