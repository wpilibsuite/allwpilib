// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Preferences.h"  // NOLINT(build/include_order)

#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include <networktables/MultiSubscriber.h>
#include <networktables/NetworkTableInstance.h>
#include <ntcore.h>
#include <units/time.h>

#include "frc/Timer.h"

static const char* kFileName = "networktables.json";
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
  preferencesFile << "[" << std::endl;
  preferencesFile << "{\"type\":\"string\","
                  << "\"name\":\"/Preferences/testFileGetString\","
                  << "\"value\":\"Hello, preferences file\","
                  << "\"properties\":{\"persistent\":true}}," << std::endl;
  preferencesFile << "{\"type\":\"int\","
                  << "\"name\":\"/Preferences/testFileGetInt\","
                  << "\"value\":1," << "\"properties\":{\"persistent\":true}},"
                  << std::endl;
  preferencesFile << "{\"type\":\"double\","
                  << "\"name\":\"/Preferences/testFileGetDouble\","
                  << "\"value\":0.5,"
                  << "\"properties\":{\"persistent\":true}}," << std::endl;
  preferencesFile << "{\"type\":\"float\","
                  << "\"name\":\"/Preferences/testFileGetFloat\","
                  << "\"value\":0.25,"
                  << "\"properties\":{\"persistent\":true}}," << std::endl;
  preferencesFile << "{\"type\":\"boolean\","
                  << "\"name\":\"/Preferences/testFileGetBoolean\","
                  << "\"value\":true,"
                  << "\"properties\":{\"persistent\":true}}]" << std::endl;
  preferencesFile.close();

  nt::MultiSubscriber suball{inst, {{std::string_view{}}}};
  inst.StartServer();

  int count = 0;
  while ((inst.GetNetworkMode() & NT_NET_MODE_STARTING) != 0) {
    frc::Wait(10_ms);
    count++;
    if (count > 30) {
      FAIL() << "timed out waiting for server startup";
    }
  }

  EXPECT_EQ("Hello, preferences file",
            frc::Preferences::GetString("testFileGetString"));
  EXPECT_EQ(1, frc::Preferences::GetInt("testFileGetInt"));
  EXPECT_FLOAT_EQ(0.5, frc::Preferences::GetDouble("testFileGetDouble"));
  EXPECT_FLOAT_EQ(0.25f, frc::Preferences::GetFloat("testFileGetFloat"));
  EXPECT_TRUE(frc::Preferences::GetBoolean("testFileGetBoolean"));
}

/**
 * If we set some values using the Preferences class, test that they show up
 * in networktables.json
 */
TEST(PreferencesTest, WritePreferencesToFile) {
  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.StartServer();

  int count = 0;
  while ((inst.GetNetworkMode() & NT_NET_MODE_STARTING) != 0) {
    frc::Wait(10_ms);
    count++;
    if (count > 30) {
      FAIL() << "timed out waiting for server startup";
    }
  }

  frc::Preferences::Remove("testFileGetString");
  frc::Preferences::Remove("testFileGetInt");
  frc::Preferences::Remove("testFileGetDouble");
  frc::Preferences::Remove("testFileGetFloat");
  frc::Preferences::Remove("testFileGetBoolean");

  frc::Wait(kSaveTime);

  frc::Preferences::SetString("testFileSetString", "Hello, preferences file");
  frc::Preferences::SetInt("testFileSetInt", 1);
  frc::Preferences::SetDouble("testFileSetDouble", 0.5);
  frc::Preferences::SetFloat("testFileSetFloat", 0.25f);
  frc::Preferences::SetBoolean("testFileSetBoolean", true);

  frc::Wait(kSaveTime);

  static char const* kExpectedFileContents[] = {
      "[",
      "  {",
      "    \"name\": \"/Preferences/testFileSetString\",",
      "    \"type\": \"string\",",
      "    \"value\": \"Hello, preferences file\",",
      "    \"properties\": {",
      "      \"persistent\": true",
      "    }",
      "  },",
      "  {",
      "    \"name\": \"/Preferences/testFileSetInt\",",
      "    \"type\": \"int\",",
      "    \"value\": 1,",
      "    \"properties\": {",
      "      \"persistent\": true",
      "    }",
      "  },",
      "  {",
      "    \"name\": \"/Preferences/testFileSetDouble\",",
      "    \"type\": \"double\",",
      "    \"value\": 0.5,",
      "    \"properties\": {",
      "      \"persistent\": true",
      "    }",
      "  },",
      "  {",
      "    \"name\": \"/Preferences/testFileSetFloat\",",
      "    \"type\": \"float\",",
      "    \"value\": 0.25,",
      "    \"properties\": {",
      "      \"persistent\": true",
      "    }",
      "  },",
      "  {",
      "    \"name\": \"/Preferences/testFileSetBoolean\",",
      "    \"type\": \"boolean\",",
      "    \"value\": true,",
      "    \"properties\": {",
      "      \"persistent\": true",
      "    }",
      "  }",
      "]"};

  std::ifstream preferencesFile(kFileName);
  for (auto& kExpectedFileContent : kExpectedFileContents) {
    ASSERT_FALSE(preferencesFile.eof())
        << "Preferences file prematurely reached EOF";

    std::string line;
    std::getline(preferencesFile, line);

    ASSERT_EQ(kExpectedFileContent, line)
        << "A line in networktables.json was not correct";
  }
}
