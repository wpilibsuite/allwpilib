// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogInputSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogInput.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AnalogInputSimTest, SetInitialized) {
  HAL_Initialize(500, 0);

  AnalogInputSim sim{5};
  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetVoltage) {
  HAL_Initialize(500, 0);

  AnalogInputSim sim{5};
  DoubleCallback callback;

  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  for (int i = 0; i < 50; ++i) {
    double voltage = i * .1;

    callback.Reset();

    sim.SetVoltage(0);
    EXPECT_NEAR(sim.GetVoltage(), 0, 0.001) << " on " << i;
    EXPECT_NEAR(input.GetVoltage(), 0, 0.001) << " on " << i;
    // 0 -> 0 isn't a change, so callback not called
    if (i > 2) {
      EXPECT_TRUE(callback.WasTriggered()) << " on " << i;
      EXPECT_EQ(0, callback.GetLastValue()) << " on " << i;
    }

    callback.Reset();
    sim.SetVoltage(voltage);
    EXPECT_NEAR(sim.GetVoltage(), voltage, 0.001) << " on " << i;
    EXPECT_NEAR(input.GetVoltage(), voltage, 0.001) << " on " << i;

    // 0 -> 0 isn't a change, so callback not called
    if (i != 0) {
      EXPECT_TRUE(callback.WasTriggered()) << " on " << i;
      EXPECT_EQ(voltage, callback.GetLastValue()) << " on " << i;
    }
  }
}

TEST(AnalogInputSimTest, SetOverSampleBits) {
  HAL_Initialize(500, 0);
  AnalogInput input{5};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb = sim.RegisterOversampleBitsCallback(callback.GetCallback(), false);

  input.SetOversampleBits(3504);
  EXPECT_EQ(3504, sim.GetOversampleBits());
  EXPECT_EQ(3504, input.GetOversampleBits());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetAverageBits) {
  HAL_Initialize(500, 0);
  AnalogInput input{5};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb = sim.RegisterAverageBitsCallback(callback.GetCallback(), false);

  input.SetAverageBits(3504);
  EXPECT_EQ(3504, sim.GetAverageBits());
  EXPECT_EQ(3504, input.GetAverageBits());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(AnalogInputSimTest, InitAccumulator) {
  HAL_Initialize(500, 0);
  AnalogInput input{0};
  AnalogInputSim sim(input);

  BooleanCallback callback;
  auto cb =
      sim.RegisterAccumulatorInitializedCallback(callback.GetCallback(), false);

  input.InitAccumulator();
  input.ResetAccumulator();
  EXPECT_TRUE(sim.GetAccumulatorInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AnalogInputSimTest, InitAccumulatorOnInvalidPort) {
  HAL_Initialize(500, 0);
  AnalogInput input{5};
  AnalogInputSim sim(input);

  BooleanCallback callback;
  auto cb =
      sim.RegisterAccumulatorInitializedCallback(callback.GetCallback(), false);

  EXPECT_THROW(input.InitAccumulator(), std::runtime_error);
  EXPECT_FALSE(callback.WasTriggered());
}

TEST(AnalogInputSimTest, SetAccumulatorValue) {
  HAL_Initialize(500, 0);
  AnalogInput input{0};
  AnalogInputSim sim(input);

  LongCallback callback;
  auto cb = sim.RegisterAccumulatorValueCallback(callback.GetCallback(), false);

  input.InitAccumulator();
  sim.SetAccumulatorValue(3504191229);
  EXPECT_EQ(3504191229, sim.GetAccumulatorValue());
  EXPECT_EQ(3504191229, input.GetAccumulatorValue());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504191229, callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetAccumulatorCount) {
  HAL_Initialize(500, 0);
  AnalogInput input{0};
  AnalogInputSim sim(input);

  LongCallback callback;
  auto cb = sim.RegisterAccumulatorCountCallback(callback.GetCallback(), false);

  input.InitAccumulator();
  sim.SetAccumulatorCount(3504191229);
  EXPECT_EQ(3504191229, sim.GetAccumulatorCount());
  EXPECT_EQ(3504191229, input.GetAccumulatorCount());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504191229, callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetAccumulatorDeadband) {
  HAL_Initialize(500, 0);
  AnalogInput input{0};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb =
      sim.RegisterAccumulatorDeadbandCallback(callback.GetCallback(), false);

  input.InitAccumulator();
  input.SetAccumulatorDeadband(3504);
  EXPECT_EQ(3504, sim.GetAccumulatorDeadband());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetAccumulatorCenter) {
  HAL_Initialize(500, 0);
  AnalogInput input{0};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb =
      sim.RegisterAccumulatorCenterCallback(callback.GetCallback(), false);

  input.InitAccumulator();
  input.SetAccumulatorCenter(3504);
  EXPECT_EQ(3504, sim.GetAccumulatorCenter());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

}  // namespace frc::sim
