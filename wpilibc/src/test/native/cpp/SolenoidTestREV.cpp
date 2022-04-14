// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"
#include "gtest/gtest.h"

namespace frc {
TEST(SolenoidREVTest, ValidInitialization) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::REVPH, 2};
  EXPECT_EQ(2, solenoid.GetChannel());

  solenoid.Set(true);
  EXPECT_TRUE(solenoid.Get());

  solenoid.Set(false);
  EXPECT_FALSE(solenoid.Get());
}

TEST(SolenoidREVTest, DoubleInitialization) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::REVPH, 2};
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::REVPH, 2),
               std::runtime_error);
}

TEST(SolenoidREVTest, DoubleInitializationFromDoubleSolenoid) {
  DoubleSolenoid solenoid{3, frc::PneumaticsModuleType::REVPH, 2, 3};
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::REVPH, 2),
               std::runtime_error);
}

TEST(SolenoidREVTest, InvalidChannel) {
  EXPECT_THROW(Solenoid(3, frc::PneumaticsModuleType::REVPH, 100),
               std::runtime_error);
}

TEST(SolenoidREVTest, Toggle) {
  Solenoid solenoid{3, frc::PneumaticsModuleType::REVPH, 2};
  solenoid.Set(true);
  EXPECT_TRUE(solenoid.Get());

  solenoid.Toggle();
  EXPECT_FALSE(solenoid.Get());

  solenoid.Toggle();
  EXPECT_TRUE(solenoid.Get());
}
}  // namespace frc
