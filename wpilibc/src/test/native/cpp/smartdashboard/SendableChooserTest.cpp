// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/simulation/SendableChooserSim.hpp"
#include "wpi/smartdashboard/SendableChooser.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

class SendableChooserTest : public ::testing::TestWithParam<int> {};

TEST_P(SendableChooserTest, ReturnsSelected) {
  wpi::SendableChooser<int> chooser;
  wpi::sim::SendableChooserSim chooserSim{
      fmt::format("/SmartDashboard/ReturnsSelectedChooser{}/", GetParam())};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }
  chooser.SetDefaultOption("0", 0);

  wpi::SmartDashboard::PutData(
      fmt::format("ReturnsSelectedChooser{}", GetParam()), &chooser);
  wpi::SmartDashboard::UpdateValues();
  chooserSim.SetSelected(std::to_string(GetParam()));
  wpi::SmartDashboard::UpdateValues();
  EXPECT_EQ(GetParam(), chooser.GetSelected());
}

TEST(SendableChooserTest, DefaultIsReturnedOnNoSelect) {
  wpi::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.SetDefaultOption("4", 4);

  EXPECT_EQ(4, chooser.GetSelected());
}

TEST(SendableChooserTest,
     DefaultConstructableIsReturnedOnNoSelectAndNoDefault) {
  wpi::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  EXPECT_EQ(0, chooser.GetSelected());
}

TEST(SendableChooserTest, ChangeListener) {
  wpi::SendableChooser<int> chooser;
  wpi::sim::SendableChooserSim chooserSim{
      "/SmartDashboard/ChangeListenerChooser/"};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  wpi::SmartDashboard::PutData("ChangeListenerChooser", &chooser);
  wpi::SmartDashboard::UpdateValues();
  chooserSim.SetSelected("3");
  wpi::SmartDashboard::UpdateValues();

  EXPECT_EQ(3, currentVal);
}

INSTANTIATE_TEST_SUITE_P(SendableChooserTests, SendableChooserTest,
                         ::testing::Values(0, 1, 2, 3));
