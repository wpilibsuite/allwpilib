// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"

namespace frc {
TEST(SolenoidCTRETest, ValidInitialization) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::CTREPCM, 2};
  EXPECT_EQ(2, solenoid.GetChannel());

  solenoid.Set(true);
  EXPECT_TRUE(solenoid.IsOn());

  solenoid.Set(false);
  EXPECT_FALSE(solenoid.IsOn());
}

TEST(SolenoidCTRETest, DoubleInitialization) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::CTREPCM, 2};
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::CTREPCM, 2),
               std::runtime_error);
}

TEST(SolenoidCTRETest, DoubleInitializationFromDoubleSolenoid) {
  DoubleSolenoid solenoid{3, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::CTREPCM, 2),
               std::runtime_error);
}

TEST(SolenoidCTRETest, InvalidChannel) {
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::CTREPCM, 100),
               std::runtime_error);
}

TEST(SolenoidCTRETest, Toggle) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::CTREPCM, 2};
  solenoid.Set(true);
  EXPECT_TRUE(solenoid.IsOn());

  solenoid.Toggle();
  EXPECT_FALSE(solenoid.IsOn());

  solenoid.Toggle();
  EXPECT_TRUE(solenoid.IsOn());
}
}  // namespace frc
