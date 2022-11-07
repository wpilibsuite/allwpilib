// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/CTREPCMSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(CTREPCMSimTest, InitializedCallback) {
  CTREPCMSim sim;

  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  PneumaticsControlModule pcm;
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(CTREPCMSimTest, SolenoidOutput) {
  PneumaticsControlModule pcm;
  CTREPCMSim sim(pcm);
  sim.ResetData();

  DoubleSolenoid doubleSolenoid{0, frc::PneumaticsModuleType::CTREPCM, 3, 4};

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
  EXPECT_EQ(0b00010000, pcm.GetSolenoids());
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
  EXPECT_EQ(0b00001000, pcm.GetSolenoids());
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
  EXPECT_EQ(0b00000000, pcm.GetSolenoids());
  EXPECT_EQ(0b00000000, sim.GetAllSolenoidOutputs());
}

TEST(CTREPCMSimTest, SetCompressorOn) {
  PneumaticsControlModule pcm;
  CTREPCMSim sim(pcm);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterCompressorOnCallback(callback.GetCallback(), false);

  EXPECT_FALSE(pcm.GetCompressor());
  EXPECT_FALSE(pcm.GetCompressor());
  sim.SetCompressorOn(true);
  EXPECT_TRUE(sim.GetCompressorOn());
  EXPECT_TRUE(pcm.GetCompressor());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(CTREPCMSimTest, SetEnableDigital) {
  PneumaticsControlModule pcm;
  CTREPCMSim sim(pcm);
  sim.ResetData();

  BooleanCallback callback;
  auto cb =
      sim.RegisterClosedLoopEnabledCallback(callback.GetCallback(), false);

  pcm.DisableCompressor();
  EXPECT_EQ(pcm.GetCompressorConfigType(), CompressorConfigType::Disabled);

  pcm.EnableCompressorDigital();
  EXPECT_TRUE(sim.GetClosedLoopEnabled());
  EXPECT_EQ(pcm.GetCompressorConfigType(), CompressorConfigType::Digital);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(CTREPCMSimTest, SetPressureSwitchEnabled) {
  PneumaticsControlModule pcm;
  CTREPCMSim sim(pcm);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterPressureSwitchCallback(callback.GetCallback(), false);

  EXPECT_FALSE(pcm.GetPressureSwitch());

  sim.SetPressureSwitch(true);
  EXPECT_TRUE(sim.GetPressureSwitch());
  EXPECT_TRUE(pcm.GetPressureSwitch());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(CTREPCMSimTest, SetCompressorCurrent) {
  PneumaticsControlModule pcm;
  CTREPCMSim sim(pcm);
  sim.ResetData();

  DoubleCallback callback;
  auto cb =
      sim.RegisterCompressorCurrentCallback(callback.GetCallback(), false);

  sim.SetCompressorCurrent(35.04);
  EXPECT_EQ(35.04, sim.GetCompressorCurrent());
  EXPECT_EQ(35.04_A, pcm.GetCompressorCurrent());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(35.04, callback.GetLastValue());
}
}  // namespace frc::sim
