// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GamepadSelectable.hpp"

#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/driverstation/DriverStationDisplay.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/GamepadSim.hpp"

using namespace wpi;

static void Tap(sim::GamepadSim& sim, GamepadSelectable& selectable,
                Gamepad::Button button) {
  sim.SetButton(button, true);
  sim.NotifyNewData();
  selectable.Update();

  sim.SetButton(button, false);
  sim.NotifyNewData();
  selectable.Update();
}

class GamepadSelectableTest : public ::testing::Test {
 protected:
  void SetUp() override {
    sim::DriverStationSim::ResetData();
    DriverStationDisplay::SetMode(DriverStationDisplay::Mode::Line);
  }

  void TearDown() override {
    wpi::internal::DriverStationBackend::ResetCachedHIDData();
  }
};

TEST_F(GamepadSelectableTest, AddOptionsSelectsFirstOptionByDefault) {
  GamepadSelectable selectable{0};

  GamepadSelectable::Chooser& chooser =
      selectable.AddOptions("Auto", {"Left", "Center", "Right"});

  EXPECT_EQ(std::string_view{"Left"}, chooser.GetSelected());
  EXPECT_EQ(std::string_view{"Left"}, selectable.GetSelected("Auto"));
  EXPECT_EQ(0, selectable.GetSelectedIndex("Auto"));
  EXPECT_EQ(std::vector<std::string>{"Auto"}, selectable.GetChooserNames());
  EXPECT_EQ(&chooser, selectable.GetSelectedChooser());
}

TEST_F(GamepadSelectableTest, AddIntegerOptionsCreatesRange) {
  GamepadSelectable selectable{0};

  GamepadSelectable::Chooser& chooser =
      selectable.AddIntegerOptions("Delay", -2, 3, 2);

  EXPECT_EQ((std::vector<std::string>{"-2", "0", "2", "3"}),
            chooser.GetOptions());
  EXPECT_EQ(std::string_view{"-2"}, chooser.GetSelected());
  EXPECT_EQ(-2, selectable.GetSelectedInteger("Delay"));
}

TEST_F(GamepadSelectableTest, AddDoubleOptionsCreatesRange) {
  GamepadSelectable selectable{0};

  GamepadSelectable::Chooser& chooser =
      selectable.AddDoubleOptions("Speed", 0.0, 1.0, 0.3);

  EXPECT_EQ((std::vector<std::string>{"0.0", "0.3", "0.6", "0.9", "1.0"}),
            chooser.GetOptions());
  EXPECT_EQ(std::string_view{"0.0"}, chooser.GetSelected());
  EXPECT_EQ(0.0, selectable.GetSelectedDouble("Speed"));
}

TEST_F(GamepadSelectableTest, DpadMovesBetweenChoosersAndOptions) {
  Gamepad gamepad{0};
  sim::GamepadSim sim{gamepad};
  GamepadSelectable selectable{gamepad};
  GamepadSelectable::Chooser& autoChooser =
      selectable.AddOptions("Auto", {"Left", "Center", "Right"});
  GamepadSelectable::Chooser& delay =
      selectable.AddIntegerOptions("Delay", 0, 2, 1);

  Tap(sim, selectable, Gamepad::Button::DPAD_RIGHT);
  EXPECT_EQ(std::string_view{"Center"}, autoChooser.GetSelected());
  EXPECT_EQ(std::string_view{"0"}, delay.GetSelected());
  EXPECT_EQ(std::string_view{"Auto"},
            selectable.GetSelectedChooser()->GetName());

  Tap(sim, selectable, Gamepad::Button::DPAD_DOWN);
  EXPECT_EQ(std::string_view{"Delay"},
            selectable.GetSelectedChooser()->GetName());

  Tap(sim, selectable, Gamepad::Button::DPAD_RIGHT);
  EXPECT_EQ(std::string_view{"1"}, delay.GetSelected());
  EXPECT_EQ(std::string_view{"Center"}, autoChooser.GetSelected());

  Tap(sim, selectable, Gamepad::Button::DPAD_LEFT);
  Tap(sim, selectable, Gamepad::Button::DPAD_LEFT);
  EXPECT_EQ(std::string_view{"2"}, delay.GetSelected());

  Tap(sim, selectable, Gamepad::Button::DPAD_UP);
  EXPECT_EQ(std::string_view{"Auto"},
            selectable.GetSelectedChooser()->GetName());

  Tap(sim, selectable, Gamepad::Button::DPAD_LEFT);
  Tap(sim, selectable, Gamepad::Button::DPAD_LEFT);
  EXPECT_EQ(std::string_view{"Right"}, autoChooser.GetSelected());
}

TEST_F(GamepadSelectableTest, RejectsInvalidChoosers) {
  GamepadSelectable selectable{0};

  EXPECT_THROW(selectable.AddOptions("", {"A"}), std::invalid_argument);
  EXPECT_THROW(selectable.AddOptions("Empty", std::vector<std::string>{}),
               std::invalid_argument);
  EXPECT_THROW(selectable.AddIntegerOptions("Bad", 0, 2, 0),
               std::invalid_argument);
  EXPECT_THROW(selectable.AddDoubleOptions("Bad", 0.0, 1.0, 0.0),
               std::invalid_argument);

  selectable.AddOptions("Auto", {"A"});
  EXPECT_THROW(selectable.AddOptions("Auto", {"B"}), std::invalid_argument);
  EXPECT_THROW(selectable.GetSelected("Missing"), std::invalid_argument);
}
