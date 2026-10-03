// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DSGamepadChooser.hpp"

#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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

class DSGamepadChooserTest {
 public:
  DSGamepadChooserTest() {
    sim::DriverStationSim::ResetData();
    DriverStationDisplay::SetMode(DriverStationDisplay::Mode::Line);
  }

  ~DSGamepadChooserTest() {
    wpi::internal::DriverStationBackend::ResetCachedHIDData();
  }
};

TEST_CASE_METHOD(DSGamepadChooserTest,
                 "DSGamepadChooserTest AddOptionsSelectsFirstOptionByDefault",
                 "[wpilibc]") {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddOptions("Auto", {"Left", "Center", "Right"});

  CHECK(std::string_view{"Left"} == selectable.GetSelected());
  CHECK(std::string_view{"Left"} == chooser.GetSelected("Auto"));
  CHECK(0 == chooser.GetSelectedIndex("Auto"));
  CHECK(std::vector<std::string>{"Auto"} == chooser.GetSelectableNames());
  CHECK(&selectable == chooser.GetSelectedSelectable());
}

TEST_CASE_METHOD(DSGamepadChooserTest,
                 "DSGamepadChooserTest AddIntegerOptionsCreatesRange",
                 "[wpilibc]") {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddIntegerOptions("Delay", -2, 3, 2);

  CHECK(std::vector<std::string>{"-2", "0", "2", "3"} ==
        selectable.GetOptions());
  CHECK(std::string_view{"-2"} == selectable.GetSelected());
  CHECK(-2 == chooser.GetSelectedInteger("Delay"));
}

TEST_CASE_METHOD(DSGamepadChooserTest,
                 "DSGamepadChooserTest AddDoubleOptionsCreatesRange",
                 "[wpilibc]") {
  DSGamepadChooser chooser{0};

  DSGamepadChooser::GamepadSelectable& selectable =
      chooser.AddDoubleOptions("Speed", 0.0, 1.0, 0.3);

  CHECK(std::vector<std::string>{"0.0", "0.3", "0.6", "0.9", "1.0"} ==
        selectable.GetOptions());
  CHECK(std::string_view{"0.0"} == selectable.GetSelected());
  CHECK(0.0 == chooser.GetSelectedDouble("Speed"));
}

TEST_CASE_METHOD(DSGamepadChooserTest,
                 "DSGamepadChooserTest DpadMovesBetweenSelectablesAndOptions",
                 "[wpilibc]") {
  Gamepad gamepad{0};
  sim::GamepadSim sim{gamepad};
  DSGamepadChooser chooser{gamepad};
  DSGamepadChooser::GamepadSelectable& autoSelectable =
      chooser.AddOptions("Auto", {"Left", "Center", "Right"});
  DSGamepadChooser::GamepadSelectable& delay =
      chooser.AddIntegerOptions("Delay", 0, 2, 1);

  Tap(sim, chooser, Gamepad::Button::DPAD_RIGHT);
  CHECK(std::string_view{"Center"} == autoSelectable.GetSelected());
  CHECK(std::string_view{"0"} == delay.GetSelected());
  CHECK(std::string_view{"Auto"} == chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_DOWN);
  CHECK(std::string_view{"Delay"} ==
        chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_RIGHT);
  CHECK(std::string_view{"1"} == delay.GetSelected());
  CHECK(std::string_view{"Center"} == autoSelectable.GetSelected());

  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  CHECK(std::string_view{"2"} == delay.GetSelected());

  Tap(sim, chooser, Gamepad::Button::DPAD_UP);
  CHECK(std::string_view{"Auto"} == chooser.GetSelectedSelectable()->GetName());

  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  Tap(sim, chooser, Gamepad::Button::DPAD_LEFT);
  CHECK(std::string_view{"Right"} == autoSelectable.GetSelected());
}

TEST_CASE_METHOD(DSGamepadChooserTest,
                 "DSGamepadChooserTest RejectsInvalidSelectables",
                 "[wpilibc]") {
  DSGamepadChooser chooser{0};

  CHECK_THROWS_AS(chooser.AddOptions("", {"A"}), std::invalid_argument);
  CHECK_THROWS_AS(chooser.AddOptions("Empty", std::vector<std::string>{}),
                  std::invalid_argument);
  CHECK_THROWS_AS(chooser.AddIntegerOptions("Bad", 0, 2, 0),
                  std::invalid_argument);
  CHECK_THROWS_AS(chooser.AddDoubleOptions("Bad", 0.0, 1.0, 0.0),
                  std::invalid_argument);

  chooser.AddOptions("Auto", {"A"});
  CHECK_THROWS_AS(chooser.AddOptions("Auto", {"B"}), std::invalid_argument);
  CHECK_THROWS_AS(chooser.GetSelected("Missing"), std::invalid_argument);
}
