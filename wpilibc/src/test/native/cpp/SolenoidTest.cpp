// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"
#include "gtest/gtest.h"

namespace frc {
TEST(SolenoidTest, ValidInitialization) {
  PneumaticsControlModule pcm(3);
  Solenoid solenoid(pcm, 2);
  EXPECT_EQ(2, solenoid.GetChannel());

  solenoid.Set(true);
  EXPECT_TRUE(solenoid.Get());

  solenoid.Set(false);
  EXPECT_FALSE(solenoid.Get());
}

TEST(SolenoidTest, DoubleInitialization) {
  PneumaticsControlModule pcm(3);
  Solenoid solenoid(&pcm, 2);
  EXPECT_THROW(Solenoid(pcm, 2), std::runtime_error);
}

TEST(SolenoidTest, DoubleInitializationFromDoubleSolenoid) {
  PneumaticsControlModule pcm(3);
  DoubleSolenoid solenoid(pcm, 2, 3);
  EXPECT_THROW(Solenoid(pcm, 2), std::runtime_error);
}

TEST(SolenoidTest, InvalidChannel) {
  PneumaticsControlModule pcm(3);
  EXPECT_THROW(Solenoid(pcm, 100), std::runtime_error);
}

TEST(SolenoidTest, Toggle) {
  PneumaticsControlModule pcm(3);
  Solenoid solenoid(pcm, 2);
  solenoid.Set(true);
  EXPECT_TRUE(solenoid.Get());

  solenoid.Toggle();
  EXPECT_FALSE(solenoid.Get());

  solenoid.Toggle();
  EXPECT_TRUE(solenoid.Get());
}
}  // namespace frc
