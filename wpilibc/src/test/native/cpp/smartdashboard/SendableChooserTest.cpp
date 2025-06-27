// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SendableChooserSim.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include <string>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

class SendableChooserTest : public ::testing::TestWithParam<int> {};

TEST_P(SendableChooserTest, ReturnsSelected) {
  frc::SendableChooser<int> chooser;
  frc::sim::SendableChooserSim chooserSim{
      fmt::format("/SmartDashboard/ReturnsSelectedChooser{}/", GetParam())};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }
  chooser.SetDefaultOption("0", 0);

  frc::SmartDashboard::PutData(
      fmt::format("ReturnsSelectedChooser{}", GetParam()), &chooser);
  frc::SmartDashboard::UpdateValues();
  chooserSim.SetSelected(std::to_string(GetParam()));
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(GetParam(), chooser.GetSelected());
  EXPECT_EQ(std::to_string(GetParam()), chooser.GetSelectedName());
}

TEST(SendableChooserTest, DefaultIsReturnedOnNoSelect) {
  frc::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.SetDefaultOption("4", 4);

  EXPECT_EQ(4, chooser.GetSelected());
  EXPECT_EQ("4", chooser.GetSelectedName());
}

TEST(SendableChooserTest,
     DefaultConstructableIsReturnedOnNoSelectAndNoDefault) {
  frc::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  EXPECT_EQ(0, chooser.GetSelected());
  EXPECT_EQ("", chooser.GetSelectedName());
}

TEST(SendableChooserTest, ChangeListener1Arg) {
  frc::SendableChooser<int> chooser;
  frc::sim::SendableChooserSim chooserSim{
      "/SmartDashboard/1ArgChangeListenerChooser/"};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }
  int currentVal = 0;
  chooser.OnChange([&](int val) { currentVal = val; });

  frc::SmartDashboard::PutData("1ArgChangeListenerChooser", &chooser);
  frc::SmartDashboard::UpdateValues();
  chooserSim.SetSelected("3");
  frc::SmartDashboard::UpdateValues();

  EXPECT_EQ(3, currentVal);
}

TEST(SendableChooserTest, ChangeListener2Arg) {
  frc::SendableChooser<int> chooser;
  frc::sim::SendableChooserSim chooserSim{
      "/SmartDashboard/2ArgChangeListenerChooser/"};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  std::string currentName = "";
  int currentVal = 0;
  chooser.OnChange([&](std::string_view name, int val) {
    currentName = std::string(name);
    currentVal = val;
  });

  frc::SmartDashboard::PutData("2ArgChangeListenerChooser", &chooser);
  frc::SmartDashboard::UpdateValues();
  chooserSim.SetSelected("3");
  frc::SmartDashboard::UpdateValues();

  EXPECT_EQ(3, currentVal);
  EXPECT_EQ("3", currentName);
}

INSTANTIATE_TEST_SUITE_P(SendableChooserTests, SendableChooserTest,
                         ::testing::Values(0, 1, 2, 3));
