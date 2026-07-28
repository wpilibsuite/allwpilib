// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DSGamepadChooser.hpp"

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

static void Tap(sim::GamepadSim& sim, DSGamepadChooser& chooser,
                Gamepad::Button button) {
  sim.SetButton(button, true);
  sim.NotifyNewData();
  chooser.Update();

  sim.SetButton(button, false);
  sim.NotifyNewData();
  chooser.Update();
}

class DSGamepadChooserTest : public ::testing::Test {
 protected:
  void SetUp() override {
    sim::DriverStationSim::ResetData();
    DriverStationDisplay::SetMode(DriverStationDisplay::Mode::Line);
  }

  void TearDown() override {
    wpi::internal::DriverStationBackend::ResetCachedHIDData();
  }
};

TEST_F(DSGamepadChooserTest, AddOptionsSelectsFirstOptionByDefault) {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddOptions("Auto", {"Left", "Center", "Right"});

  EXPECT_EQ(std::string_view{"Left"}, selectable.GetSelected());
  EXPECT_EQ(std::string_view{"Left"}, chooser.GetSelected("Auto"));
  EXPECT_EQ(0, chooser.GetSelectedIndex("Auto"));
  EXPECT_EQ(std::vector<std::string>{"Auto"}, chooser.GetSelectableNames());
  EXPECT_EQ(&selectable, chooser.GetSelectedSelectable());
}

TEST_F(DSGamepadChooserTest, AddIntegerOptionsCreatesRange) {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddIntegerOptions("Delay", -2, 3, 2);

  EXPECT_EQ((std::vector<std::string>{"-2", "0", "2", "3"}),
            selectable.GetOptions());
  EXPECT_EQ(std::string_view{"-2"}, selectable.GetSelected());
  EXPECT_EQ(-2, chooser.GetSelectedInteger("Delay"));
}

TEST_F(DSGamepadChooserTest, AddDoubleOptionsCreatesRange) {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddDoubleOptions("Speed", 0.0, 1.0, 0.3);

  EXPECT_EQ((std::vector<std::string>{"0.0", "0.3", "0.6", "0.9", "1.0"}),
            selectable.GetOptions());
  EXPECT_EQ(std::string_view{"0.0"}, selectable.GetSelected());
  EXPECT_EQ(0.0, chooser.GetSelectedDouble("Speed"));
}

TEST_F(DSGamepadChooserTest, DpadMovesBetweenSelectablesAndOptions) {
  Gamepad gamepad{0};
  sim::GamepadSim sim{gamepad};
  DSGamepadChooser chooser{gamepad};
  DSGamepadChooser::GamepadSelectable& autoSelectable =
      chooser.AddOptions("Auto", {"Left", "Center", "Right"});
  DSGamepadChooser::GamepadSelectable& delay =
      chooser.AddIntegerOptions("Delay", 0, 2, 1);

  Tap(sim, chooser, Gamepad::Button::DPAD_RIGHT);
  EXPECT_EQ(std::string_view{"Center"}, autoSelectable.GetSelected());
  EXPECT_EQ(std::string_view{"0"}, delay.GetSelected());
  EXPECT_EQ(std::string_view{"Auto"},
            chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_DOWN);
  EXPECT_EQ(std::string_view{"Delay"},
            chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_RIGHT);
  EXPECT_EQ(std::string_view{"1"}, delay.GetSelected());
  EXPECT_EQ(std::string_view{"Center"}, autoSelectable.GetSelected());

  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  EXPECT_EQ(std::string_view{"2"}, delay.GetSelected());

  Tap(sim, chooser, Gamepad::Button::DPAD_UP);
  EXPECT_EQ(std::string_view{"Auto"},
            chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  EXPECT_EQ(std::string_view{"Right"}, autoSelectable.GetSelected());
}

TEST_F(DSGamepadChooserTest, RejectsInvalidSelectables) {
  DSGamepadChooser chooser{0};

  EXPECT_THROW(chooser.AddOptions("", {"A"}), std::invalid_argument);
  EXPECT_THROW(chooser.AddOptions("Empty", std::vector<std::string>{}),
               std::invalid_argument);
  EXPECT_THROW(chooser.AddIntegerOptions("Bad", 0, 2, 0),
               std::invalid_argument);
  EXPECT_THROW(chooser.AddDoubleOptions("Bad", 0.0, 1.0, 0.0),
               std::invalid_argument);

  chooser.AddOptions("Auto", {"A"});
  EXPECT_THROW(chooser.AddOptions("Auto", {"B"}), std::invalid_argument);
  EXPECT_THROW(chooser.GetSelected("Missing"), std::invalid_argument);
}
