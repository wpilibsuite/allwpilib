// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/REVPHSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticHub.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(REVPHSimTest, InitializedCallback) {
  REVPHSim sim;

  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  PneumaticHub ph;
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(REVPHSimTest, SolenoidOutput) {
  PneumaticHub ph;
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
  PneumaticHub ph;
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

TEST(REVPHSimTest, SetEnableDigital) {
  PneumaticHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  EnumCallback callback;
  auto cb =
      sim.RegisterCompressorConfigTypeCallback(callback.GetCallback(), false);

  ph.DisableCompressor();
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Disabled);

  ph.EnableCompressorDigital();
  EXPECT_EQ(sim.GetCompressorConfigType(),
            static_cast<int>(CompressorConfigType::Digital));
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Digital);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(callback.GetLastValue(),
            static_cast<int>(CompressorConfigType::Digital));
}

TEST(REVPHSimTest, SetEnableAnalog) {
  PneumaticHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  EnumCallback callback;
  auto cb =
      sim.RegisterCompressorConfigTypeCallback(callback.GetCallback(), false);

  ph.DisableCompressor();
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Disabled);

  ph.EnableCompressorAnalog(1_psi, 2_psi);
  EXPECT_EQ(sim.GetCompressorConfigType(),
            static_cast<int>(CompressorConfigType::Analog));
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Analog);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(callback.GetLastValue(),
            static_cast<int>(CompressorConfigType::Analog));
}

TEST(REVPHSimTest, SetEnableHybrid) {
  PneumaticHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  EnumCallback callback;
  auto cb =
      sim.RegisterCompressorConfigTypeCallback(callback.GetCallback(), false);

  ph.DisableCompressor();
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Disabled);

  ph.EnableCompressorHybrid(1_psi, 2_psi);
  EXPECT_EQ(sim.GetCompressorConfigType(),
            static_cast<int>(CompressorConfigType::Hybrid));
  EXPECT_EQ(ph.GetCompressorConfigType(), CompressorConfigType::Hybrid);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(callback.GetLastValue(),
            static_cast<int>(CompressorConfigType::Hybrid));
}

TEST(REVPHSimTest, SetPressureSwitchEnabled) {
  PneumaticHub ph;
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
  PneumaticHub ph;
  REVPHSim sim(ph);
  sim.ResetData();

  DoubleCallback callback;
  auto cb =
      sim.RegisterCompressorCurrentCallback(callback.GetCallback(), false);

  sim.SetCompressorCurrent(35.04);
  EXPECT_EQ(35.04, sim.GetCompressorCurrent());
  EXPECT_EQ(35.04_A, ph.GetCompressorCurrent());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(35.04, callback.GetLastValue());
}
}  // namespace frc::sim
