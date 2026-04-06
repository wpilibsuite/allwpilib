// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/SendableChooser.hpp"

#include <string>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "wpi/simulation/SendableChooserSim.hpp"
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

TEST(SendableChooserTest, ClearResetsChooser) {
  SendableChooser<int> chooser;
  sim::SendableChooserSim chooserSim{"/SmartDashboard/ClearChooser/"};

  chooser.AddOption("1", 1);
  chooser.AddOption("2", 2);
  chooser.SetDefaultOption("0", 0);

  int listenerVal = -1;
  chooser.OnChange([&](int val) { listenerVal = val; });

  chooserSim.SetSelected("1");
  SmartDashboard::UpdateValues();
  EXPECT_EQ(1, chooser.GetSelected());
  EXPECT_EQ(1, listenerVal);


  chooser.Clear();

  listenerVal = -1;

  EXPECT_EQ(0, chooser.GetSelected());

  chooserSim.SetSelected("1");
  SmartDashboard::UpdateValues();

  EXPECT_EQ(-1, listenerVal);

  chooser.AddOption("3", 3);
  chooser.SetDefaultOption("5", 5);
  EXPECT_EQ(5, chooser.GetSelected());
}

INSTANTIATE_TEST_SUITE_P(SendableChooserTests, SendableChooserTest,
                         ::testing::Values(0, 1, 2, 3));
