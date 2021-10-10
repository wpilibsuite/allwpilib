// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"
#include "gtest/gtest.h"

namespace frc {

TEST(DoubleSolenoidCTRETest, ValidInitialization) {
  DoubleSolenoid solenoid{3, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  solenoid.Set(DoubleSolenoid::kReverse);
  EXPECT_EQ(DoubleSolenoid::kReverse, solenoid.Get());

  solenoid.Set(DoubleSolenoid::kForward);
  EXPECT_EQ(DoubleSolenoid::kForward, solenoid.Get());

  solenoid.Set(DoubleSolenoid::kOff);
  EXPECT_EQ(DoubleSolenoid::kOff, solenoid.Get());
}

TEST(DoubleSolenoidCTRETest, ThrowForwardPortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{5, frc::PneumaticsModuleType::CTREPCM, 2};
  EXPECT_THROW(DoubleSolenoid(5, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, ThrowReversePortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{6, frc::PneumaticsModuleType::CTREPCM, 3};
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, ThrowBothPortsAlreadyInitialized) {
  PneumaticsControlModule pcm{6};
  // Single solenoid that is reused for forward port
  Solenoid solenoid0(6, frc::PneumaticsModuleType::CTREPCM, 2);
  Solenoid solenoid1(6, frc::PneumaticsModuleType::CTREPCM, 3);
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, Toggle) {
  DoubleSolenoid solenoid{4, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  // Bootstrap it into reverse
  solenoid.Set(DoubleSolenoid::kReverse);

  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::kForward, solenoid.Get());

  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::kReverse, solenoid.Get());

  // Of shouldn't do anything on toggle
  solenoid.Set(DoubleSolenoid::kOff);
  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::kOff, solenoid.Get());
}

TEST(DoubleSolenoidCTRETest, InvalidForwardPort) {
  EXPECT_THROW(DoubleSolenoid(0, frc::PneumaticsModuleType::CTREPCM, 100, 1),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, InvalidReversePort) {
  EXPECT_THROW(DoubleSolenoid(0, frc::PneumaticsModuleType::CTREPCM, 0, 100),
               std::runtime_error);
}
}  // namespace frc
