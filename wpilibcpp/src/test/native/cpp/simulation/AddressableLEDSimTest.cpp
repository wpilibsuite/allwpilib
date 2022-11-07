// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AddressableLEDSim.h"  // NOLINT(build/include_order)

#include <array>

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AddressableLED.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AddressableLEDSimTest, InitializationCallback) {
  HAL_Initialize(500, 0);

  BooleanCallback callback;
  AddressableLEDSim sim;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  EXPECT_FALSE(callback.WasTriggered());
  AddressableLED led{0};

  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AddressableLEDSimTest, SetLength) {
  HAL_Initialize(500, 0);

  AddressableLED led{0};
  AddressableLEDSim sim{led};
  IntCallback callback;

  auto cb = sim.RegisterLengthCallback(callback.GetCallback(), false);

  EXPECT_EQ(1, sim.GetLength());  // Defaults to 1 led

  std::array<AddressableLED::LEDData, 50> ledData;
  led.SetLength(ledData.max_size());
  led.SetData(ledData);

  EXPECT_EQ(50, sim.GetLength());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(50, callback.GetLastValue());
}

TEST(AddressableLEDSimTest, SetRunning) {
  HAL_Initialize(500, 0);

  AddressableLEDSim sim = AddressableLEDSim::CreateForIndex(0);
  BooleanCallback callback;
  auto cb = sim.RegisterRunningCallback(callback.GetCallback(), false);
  AddressableLED led{0};

  EXPECT_FALSE(sim.GetRunning());

  led.Start();
  EXPECT_TRUE(sim.GetRunning());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  callback.Reset();
  led.Stop();
  EXPECT_FALSE(sim.GetRunning());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(AddressableLEDSimTest, SetData) {
  AddressableLED led{0};
  AddressableLEDSim sim = AddressableLEDSim::CreateForChannel(0);

  bool callbackHit = false;
  std::array<AddressableLED::LEDData, 3> setData;
  auto cb = sim.RegisterDataCallback(
      [&](std::string_view, const unsigned char* buffer, unsigned int count) {
        ASSERT_EQ(count, 12u);
        EXPECT_EQ(0, buffer[0]);
        EXPECT_EQ(0, buffer[1]);
        EXPECT_EQ(255u, buffer[2]);
        EXPECT_EQ(0, buffer[3]);

        EXPECT_EQ(0, buffer[4]);
        EXPECT_EQ(255u, buffer[5]);
        EXPECT_EQ(0, buffer[6]);
        EXPECT_EQ(0, buffer[7]);

        EXPECT_EQ(255u, buffer[8]);
        EXPECT_EQ(0, buffer[9]);
        EXPECT_EQ(0, buffer[10]);
        EXPECT_EQ(0, buffer[11]);

        callbackHit = true;
      },
      false);

  std::array<AddressableLED::LEDData, 3> ledData;
  led.SetLength(ledData.max_size());

  ledData[0].SetRGB(255, 0, 0);
  ledData[1].SetRGB(0, 255, 0);
  ledData[2].SetRGB(0, 0, 255);
  led.SetData(ledData);

  EXPECT_TRUE(callbackHit);

  std::array<HAL_AddressableLEDData, 3> simData;
  sim.GetData(simData.data());

  EXPECT_EQ(0xFF, simData[0].r);
  EXPECT_EQ(0x00, simData[0].g);
  EXPECT_EQ(0x00, simData[0].b);
  EXPECT_EQ(0x00, simData[0].padding);

  EXPECT_EQ(0x00, simData[1].r);
  EXPECT_EQ(0xFF, simData[1].g);
  EXPECT_EQ(0x00, simData[1].b);
  EXPECT_EQ(0x00, simData[1].padding);

  EXPECT_EQ(0x00, simData[2].r);
  EXPECT_EQ(0x00, simData[2].g);
  EXPECT_EQ(0xFF, simData[2].b);
  EXPECT_EQ(0x00, simData[2].padding);
}

}  // namespace frc::sim
