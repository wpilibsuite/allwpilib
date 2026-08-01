// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AddressableLEDSim.hpp"

#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/led/AddressableLED.hpp"

namespace wpi::sim {

TEST_CASE("AddressableLEDSimTest InitializationCallback",
          "[wpilibc][simulation]") {
  HAL_Initialize();

  BooleanCallback callback;
  AddressableLEDSim sim{0};
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  CHECK_FALSE(callback.WasTriggered());
  AddressableLED led{0};

  CHECK(sim.GetInitialized());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("AddressableLEDSimTest SetStart", "[wpilibc][simulation]") {
  HAL_Initialize();

  AddressableLED led{0};
  AddressableLEDSim sim{led};
  IntCallback callback;

  auto cb = sim.RegisterStartCallback(callback.GetCallback(), false);

  CHECK((0) == (sim.GetStart()));  // Defaults to 0

  std::array<AddressableLED::LEDData, 50> ledData;
  led.SetStart(1);
  led.SetData(ledData);

  CHECK((1) == (sim.GetStart()));
  CHECK(callback.WasTriggered());
  CHECK((1) == (callback.GetLastValue()));
}

TEST_CASE("AddressableLEDSimTest SetLength", "[wpilibc][simulation]") {
  HAL_Initialize();

  AddressableLED led{0};
  AddressableLEDSim sim{led};
  IntCallback callback;

  auto cb = sim.RegisterLengthCallback(callback.GetCallback(), false);

  CHECK((0) == (sim.GetLength()));  // Defaults to 0 leds

  std::array<AddressableLED::LEDData, 50> ledData;
  led.SetLength(ledData.max_size());
  led.SetData(ledData);

  CHECK((50) == (sim.GetLength()));
  CHECK(callback.WasTriggered());
  CHECK((50) == (callback.GetLastValue()));
}

TEST_CASE("AddressableLEDSimTest SetData", "[wpilibc][simulation]") {
  AddressableLED led{0};
  AddressableLEDSim sim{0};

  bool callbackHit = false;
  std::array<AddressableLED::LEDData, 3> setData;
  auto cb = sim.RegisterDataCallback(
      [&](std::string_view, const unsigned char* buffer, unsigned int count) {
        REQUIRE((count) == (9u));
        CHECK((255u) == (buffer[0]));
        CHECK((0) == (buffer[1]));
        CHECK((0) == (buffer[2]));

        CHECK((0) == (buffer[3]));
        CHECK((255u) == (buffer[4]));
        CHECK((0) == (buffer[5]));

        CHECK((0) == (buffer[6]));
        CHECK((0) == (buffer[7]));
        CHECK((255u) == (buffer[8]));

        callbackHit = true;
      },
      false);

  std::array<AddressableLED::LEDData, 3> ledData;
  led.SetLength(ledData.max_size());

  ledData[0].SetRGB(255, 0, 0);
  ledData[1].SetRGB(0, 255, 0);
  ledData[2].SetRGB(0, 0, 255);
  led.SetData(ledData);

  CHECK(callbackHit);

  std::array<HAL_AddressableLEDData, 3> simData;
  sim.GetData(simData.data());

  CHECK((0xFF) == (simData[0].r));
  CHECK((0x00) == (simData[0].g));
  CHECK((0x00) == (simData[0].b));

  CHECK((0x00) == (simData[1].r));
  CHECK((0xFF) == (simData[1].g));
  CHECK((0x00) == (simData[1].b));

  CHECK((0x00) == (simData[2].r));
  CHECK((0x00) == (simData[2].g));
  CHECK((0xFF) == (simData[2].b));
}

}  // namespace wpi::sim
