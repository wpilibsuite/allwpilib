// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>
#include <wpi/util/Color8Bit.hpp>

TEST(Color8BitTest, ConstructDefault) {
  constexpr wpi::Color8Bit color;

  EXPECT_EQ(0, color.red);
  EXPECT_EQ(0, color.green);
  EXPECT_EQ(0, color.blue);
}

TEST(Color8BitTest, ConstructFromInts) {
  constexpr wpi::Color8Bit color{255, 128, 64};

  EXPECT_EQ(255, color.red);
  EXPECT_EQ(128, color.green);
  EXPECT_EQ(64, color.blue);
}

TEST(Color8BitTest, ConstructFromColor) {
  constexpr wpi::Color8Bit color{wpi::Color{255, 128, 64}};

  EXPECT_EQ(255, color.red);
  EXPECT_EQ(128, color.green);
  EXPECT_EQ(64, color.blue);
}

TEST(Color8BitTest, ConstructFromHexString) {
  constexpr wpi::Color8Bit color{"#FF8040"};

  EXPECT_EQ(255, color.red);
  EXPECT_EQ(128, color.green);
  EXPECT_EQ(64, color.blue);

  // No leading #
  EXPECT_THROW(wpi::Color8Bit{"112233"}, std::invalid_argument);

  // Too long
  EXPECT_THROW(wpi::Color8Bit{"#11223344"}, std::invalid_argument);

  // Invalid hex characters
  EXPECT_THROW(wpi::Color8Bit{"#$$$$$$"}, std::invalid_argument);
}

TEST(Color8BitTest, ImplicitConversionToColor) {
  wpi::Color color = wpi::Color8Bit{255, 128, 64};

  EXPECT_NEAR(1.0, color.red, 1e-2);
  EXPECT_NEAR(0.5, color.green, 1e-2);
  EXPECT_NEAR(0.25, color.blue, 1e-2);
}

TEST(Color8BitTest, ToHexString) {
  constexpr wpi::Color8Bit color1{255, 128, 64};
  EXPECT_EQ("#FF8040", color1.HexString());

  // Ensure conversion to std::string works
  [[maybe_unused]]
  std::string str = color1.HexString();

  wpi::Color8Bit color2{255, 128, 64};
  EXPECT_EQ("#FF8040", color2.HexString());
}
