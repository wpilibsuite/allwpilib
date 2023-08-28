// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>

#include <fmt/format.h>
#include <frc/AddressableLED.h>
#include <frc/simulation/AddressableLEDSim.h>
#include <frc/util/Color.h>
#include <frc/util/Color8Bit.h>
#include <gtest/gtest.h>
#include <hal/AddressableLEDTypes.h>

#include "Robot.h"

void AssertIndexColor(std::array<frc::AddressableLED::LEDData, 60> data,
                      int index, int hue, int saturation, int value);

TEST(RainbowTest, RainbowPattern) {
  Robot robot;
  robot.RobotInit();
  frc::sim::AddressableLEDSim ledSim =
      frc::sim::AddressableLEDSim::CreateForChannel(9);
  EXPECT_TRUE(ledSim.GetRunning());
  EXPECT_EQ(60, ledSim.GetLength());

  auto rainbowFirstPixelHue = 0;
  std::array<frc::AddressableLED::LEDData, 60> data;
  for (int iteration = 0; iteration < 100; iteration++) {
    SCOPED_TRACE(fmt::format("Iteration {} of 100", iteration));
    robot.RobotPeriodic();
    ledSim.GetData(data.data());
    for (int i = 0; i < 60; i++) {
      SCOPED_TRACE(fmt::format("LED {} of 60", i));
      auto hue = (rainbowFirstPixelHue + (i * 180 / 60)) % 180;
      AssertIndexColor(data, i, hue, 255, 128);
    }
    rainbowFirstPixelHue += 3;
    rainbowFirstPixelHue %= 180;
  }
}

void AssertIndexColor(std::array<frc::AddressableLED::LEDData, 60> data,
                      int index, int hue, int saturation, int value) {
  frc::Color8Bit color{frc::Color::FromHSV(hue, saturation, value)};

  EXPECT_EQ(0, data[index].padding);
  EXPECT_EQ(color.red, data[index].r & 0xFF);
  EXPECT_EQ(color.green, data[index].g & 0xFF);
  EXPECT_EQ(color.blue, data[index].b & 0xFF);
}
