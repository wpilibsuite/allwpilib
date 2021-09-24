// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/REVPHSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsHub.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(REVPHSimTest, InitializedCallback) {
  REVPHSim sim;

  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  PneumaticsHub ph;
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(REVPHSimTest, SolenoidOutput) {
  PneumaticsHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  DoubleSolenoid doubleSolenoid{1, frc::PneumaticsModuleType::REVPH, 3, 4};

  BooleanCallback callback3;
  BooleanCallback callback4;
  auto cb3 =
      sim.RegisterSolenoidOutputCallback(3, callback3.GetCallback(), false);
  auto cb4 =
      sim.RegisterSolenoidOutputCallback(4, callback4.GetCallback(), false);

  callback3.Reset();
  callback4.Reset();
  doubleSolenoid.Set(DoubleSolenoid::kReverse);
  EXPECT_FALSE(callback3.WasTriggered());
  EXPECT_FALSE(callback3.GetLastValue());
  EXPECT_TRUE(callback4.WasTriggered());
  EXPECT_TRUE(callback4.GetLastValue());
  EXPECT_FALSE(sim.GetSolenoidOutput(3));
  EXPECT_TRUE(sim.GetSolenoidOutput(4));
  EXPECT_EQ(0b00010000, ph.GetSolenoids());
  EXPECT_EQ(0b00010000, sim.GetAllSolenoidOutputs());

  callback3.Reset();
  callback4.Reset();
  doubleSolenoid.Set(DoubleSolenoid::kForward);
  EXPECT_TRUE(callback3.WasTriggered());
  EXPECT_TRUE(callback3.GetLastValue());
  EXPECT_TRUE(callback4.WasTriggered());
  EXPECT_FALSE(callback4.GetLastValue());
  EXPECT_TRUE(sim.GetSolenoidOutput(3));
  EXPECT_FALSE(sim.GetSolenoidOutput(4));
  EXPECT_EQ(0b00001000, ph.GetSolenoids());
  EXPECT_EQ(0b00001000, sim.GetAllSolenoidOutputs());

  callback3.Reset();
  callback4.Reset();
  doubleSolenoid.Set(DoubleSolenoid::kOff);
  EXPECT_TRUE(callback3.WasTriggered());
  EXPECT_FALSE(callback3.GetLastValue());
  EXPECT_FALSE(callback4.WasTriggered());
  EXPECT_FALSE(callback4.GetLastValue());
  EXPECT_FALSE(sim.GetSolenoidOutput(3));
  EXPECT_FALSE(sim.GetSolenoidOutput(4));
  EXPECT_EQ(0b00000000, ph.GetSolenoids());
  EXPECT_EQ(0b00000000, sim.GetAllSolenoidOutputs());
}

TEST(REVPHSimTest, SetCompressorOn) {
  PneumaticsHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterCompressorOnCallback(callback.GetCallback(), false);

  EXPECT_FALSE(ph.GetCompressor());
  EXPECT_FALSE(ph.GetCompressor());
  sim.SetCompressorOn(true);
  EXPECT_TRUE(sim.GetCompressorOn());
  EXPECT_TRUE(ph.GetCompressor());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(REVPHSimTest, SetClosedLoopEnabled) {
  PneumaticsHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  BooleanCallback callback;
  auto cb =
      sim.RegisterClosedLoopEnabledCallback(callback.GetCallback(), false);

  ph.SetClosedLoopControl(false);
  EXPECT_FALSE(ph.GetClosedLoopControl());

  ph.SetClosedLoopControl(true);
  EXPECT_TRUE(sim.GetClosedLoopEnabled());
  EXPECT_TRUE(ph.GetClosedLoopControl());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(REVPHSimTest, SetPressureSwitchEnabled) {
  PneumaticsHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterPressureSwitchCallback(callback.GetCallback(), false);

  EXPECT_FALSE(ph.GetPressureSwitch());

  sim.SetPressureSwitch(true);
  EXPECT_TRUE(sim.GetPressureSwitch());
  EXPECT_TRUE(ph.GetPressureSwitch());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(REVPHSimTest, SetCompressorCurrent) {
  PneumaticsHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  DoubleCallback callback;
  auto cb =
      sim.RegisterCompressorCurrentCallback(callback.GetCallback(), false);

  sim.SetCompressorCurrent(35.04);
  EXPECT_EQ(35.04, sim.GetCompressorCurrent());
  EXPECT_EQ(35.04, ph.GetCompressorCurrent());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(35.04, callback.GetLastValue());
}
}  // namespace frc::sim
