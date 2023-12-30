// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/StringExtras.h"

TEST(StringExtrasTest, RemovePrefix) {
  std::string_view original = "wpilib";
  std::string_view modified = wpi::remove_prefix(original, "wpi");
  EXPECT_EQ(original, "wpilib");
  EXPECT_EQ(modified, "lib");
}

TEST(StringExtrasTest, RemoveSuffix) {
  std::string_view original = "wpilib";
  EXPECT_TRUE(original.ends_with("lib"));
  std::string_view modified = wpi::remove_suffix(original, "lib");
  EXPECT_EQ(original, "wpilib");
  EXPECT_EQ(modified, "wpi");
}

TEST(StringExtrasTest, RemovePrefixNoMatch) {
  std::string_view original = "wpilib";
  std::string_view modified = wpi::remove_prefix(original, "foo");
  EXPECT_EQ(original, "wpilib");
  EXPECT_EQ(modified, "wpilib");
}

TEST(StringExtrasTest, RemoveSuffixNoMatch) {
  std::string_view original = "wpilib";
  std::string_view modified = wpi::remove_suffix(original, "foo");
  EXPECT_EQ(original, "wpilib");
  EXPECT_EQ(modified, "wpilib");
}
