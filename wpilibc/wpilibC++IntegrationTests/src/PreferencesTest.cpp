/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include <cstdio>
#include <fstream>

static const char *kFileName = "wpilib-preferences.ini";
static const double kSaveTime = 0.2;

/**
 * If we write a new wpilib-preference.ini with some sample values, test that
 * we get those same values back using the Preference class.
 */
TEST(PreferencesTest, ReadPreferencesFromFile) {
	std::remove(kFileName);
	std::ofstream preferencesFile(kFileName);
	preferencesFile << "[Preferences]" << std::endl;
	preferencesFile << "testFileGetString=\"Hello, preferences file\"" << std::endl;
	preferencesFile << "testFileGetInt=\"1\"" << std::endl;
	preferencesFile << "testFileGetDouble=\"0.5\"" << std::endl;
	preferencesFile << "testFileGetFloat=\"0.25\"" << std::endl;
	preferencesFile << "testFileGetBoolean=\"true\"" << std::endl;
	preferencesFile << "testFileGetLong=\"1000000000000000000\"" << std::endl;
	preferencesFile.close();

	Preferences *preferences = Preferences::GetInstance();
	EXPECT_EQ("Hello, preferences file", preferences->GetString("testFileGetString"));
	EXPECT_EQ(1, preferences->GetInt("testFileGetInt"));
	EXPECT_FLOAT_EQ(0.5, preferences->GetDouble("testFileGetDouble"));
	EXPECT_FLOAT_EQ(0.25f, preferences->GetFloat("testFileGetFloat"));
	EXPECT_TRUE(preferences->GetBoolean("testFileGetBoolean"));
	EXPECT_EQ(1000000000000000000ll, preferences->GetLong("testFileGetLong"));
}

/**
 * If we set some values using the Preferences class, test that they show up
 * in wpilib-preferences.ini
 */
TEST(PreferencesTest, WritePreferencesToFile) {
	Preferences *preferences = Preferences::GetInstance();
	preferences->PutString("testFilePutString", "Hello, preferences file");
	preferences->PutInt("testFilePutInt", 1);
	preferences->PutDouble("testFilePutDouble", 0.5);
	preferences->PutFloat("testFilePutFloat", 0.25f);
	preferences->PutBoolean("testFilePutBoolean", true);
	preferences->PutLong("testFilePutLong", 1000000000000000000ll);
	preferences->Save();

	Wait(kSaveTime);

	static char const *kExpectedFileContents[] = {
		"[Preferences]",
		"testFileGetString=\"Hello, preferences file\"",
		"testFileGetInt=\"1\"",
		"testFileGetDouble=\"0.5\"",
		"testFileGetFloat=\"0.25\"",
		"testFileGetBoolean=\"true\"",
		"testFileGetLong=\"1000000000000000000\""
	};

	std::ifstream preferencesFile(kFileName);
	for(int i = 0; i < 7; i++) {
		ASSERT_FALSE(preferencesFile.eof())
			<< "Preferences file prematurely reached EOF";

		std::string line;
		std::getline(preferencesFile, line);

		ASSERT_EQ(kExpectedFileContents[i], line)
			<< "A line in wpilib-preferences.ini was not correct";
	}
}
