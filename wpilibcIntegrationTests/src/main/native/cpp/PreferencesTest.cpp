// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Preferences.h"  // NOLINT(build/include_order)

#include <cstdio>
#include <fstream>

#include <networktables/NetworkTableInstance.h>
#include <ntcore.h>

#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const char* kFileName = "networktables.ini";
static const double kSaveTime = 1.2;

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

  Preferences* preferences = Preferences::GetInstance();
  EXPECT_EQ("Hello, preferences file",
            preferences->GetString("testFileGetString"));
  EXPECT_EQ(1, preferences->GetInt("testFileGetInt"));
  EXPECT_FLOAT_EQ(0.5, preferences->GetDouble("testFileGetDouble"));
  EXPECT_FLOAT_EQ(0.25f, preferences->GetFloat("testFileGetFloat"));
  EXPECT_TRUE(preferences->GetBoolean("testFileGetBoolean"));
  EXPECT_EQ(1000000000000000000ll, preferences->GetLong("testFileGetLong"));
}

/**
 * If we set some values using the Preferences class, test that they show up
 * in networktables.ini
 */
TEST(PreferencesTest, WritePreferencesToFile) {
  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.StartServer();
  Preferences* preferences = Preferences::GetInstance();
  preferences->Remove("testFileGetString");
  preferences->Remove("testFileGetInt");
  preferences->Remove("testFileGetDouble");
  preferences->Remove("testFileGetFloat");
  preferences->Remove("testFileGetBoolean");
  preferences->Remove("testFileGetLong");

  Wait(kSaveTime);

  preferences->PutString("testFilePutString", "Hello, preferences file");
  preferences->PutInt("testFilePutInt", 1);
  preferences->PutDouble("testFilePutDouble", 0.5);
  preferences->PutFloat("testFilePutFloat", 0.25f);
  preferences->PutBoolean("testFilePutBoolean", true);
  preferences->PutLong("testFilePutLong", 1000000000000000000ll);

  Wait(kSaveTime);

  static char const* kExpectedFileContents[] = {
      "[NetworkTables Storage 3.0]",
      "string \"/Preferences/.type\"=\"RobotPreferences\"",
      "boolean \"/Preferences/testFilePutBoolean\"=true",
      "double \"/Preferences/testFilePutDouble\"=0.5",
      "double \"/Preferences/testFilePutFloat\"=0.25",
      "double \"/Preferences/testFilePutInt\"=1",
      "double \"/Preferences/testFilePutLong\"=1e+18",
      "string \"/Preferences/testFilePutString\"=\"Hello, preferences file\""};

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
