/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Preferences.h"  // NOLINT(build/include_order)

#include <cstdio>
#include <fstream>

#include "Timer.h"
#include "gtest/gtest.h"
#include "ntcore.h"

using namespace frc;

static const char* kFileName = "networktables.ini";
static const double kSaveTime = 1.2;

/**
 * If we write a new networktables.ini with some sample values, test that
 * we get those same values back using the Preference class.
 */
TEST(PreferencesTest, ReadPreferencesFromFile) {
  NetworkTable::Shutdown();
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
  NetworkTable::Initialize();

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
  NetworkTable::Shutdown();
  NetworkTable::GlobalDeleteAll();
  // persistent keys don't get deleted normally, so make remaining keys
  // non-persistent and delete them too
  for (const auto& info : nt::GetEntryInfo("", 0)) {
    nt::SetEntryFlags(info.name, 0);
  }
  NetworkTable::GlobalDeleteAll();
  std::remove(kFileName);
  NetworkTable::Initialize();
  Preferences* preferences = Preferences::GetInstance();
  preferences->PutString("testFilePutString", "Hello, preferences file");
  preferences->PutInt("testFilePutInt", 1);
  preferences->PutDouble("testFilePutDouble", 0.5);
  preferences->PutFloat("testFilePutFloat", 0.25f);
  preferences->PutBoolean("testFilePutBoolean", true);
  preferences->PutLong("testFilePutLong", 1000000000000000000ll);

  Wait(kSaveTime);

  static char const* kExpectedFileContents[] = {
      "[NetworkTables Storage 3.0]",
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
