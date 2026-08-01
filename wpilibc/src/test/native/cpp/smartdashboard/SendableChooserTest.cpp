// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/SendableChooser.hpp"

#include <format>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "wpi/simulation/SendableChooserSim.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

TEST_CASE("SendableChooserTest returns selected", "[wpilibc][smartdashboard]") {
  auto selected = GENERATE(0, 1, 2, 3);

  wpi::SendableChooser<int> chooser;
  wpi::sim::SendableChooserSim chooserSim{
      std::format("/SmartDashboard/ReturnsSelectedChooser{}/", selected)};

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }
  chooser.SetDefaultOption("0", 0);

  wpi::SmartDashboard::PutData(
      std::format("ReturnsSelectedChooser{}", selected), &chooser);
  wpi::SmartDashboard::UpdateValues();
  chooserSim.SetSelected(std::to_string(selected));
  wpi::SmartDashboard::UpdateValues();
  CHECK(selected == chooser.GetSelected());
}

TEST_CASE("SendableChooserTest default is returned on no select",
          "[wpilibc][smartdashboard]") {
  wpi::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  // Use 4 here rather than 0 to make sure it's not default-init int.
  chooser.SetDefaultOption("4", 4);

  CHECK(4 == chooser.GetSelected());
}

TEST_CASE(
    "SendableChooserTest default constructible is returned on no select "
    "and no default",
    "[wpilibc][smartdashboard]") {
  wpi::SendableChooser<int> chooser;

  for (int i = 1; i <= 3; i++) {
    chooser.AddOption(std::to_string(i), i);
  }

  CHECK(0 == chooser.GetSelected());
}

TEST_CASE("SendableChooserTest change listener", "[wpilibc][smartdashboard]") {
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

  CHECK(3 == currentVal);
}
