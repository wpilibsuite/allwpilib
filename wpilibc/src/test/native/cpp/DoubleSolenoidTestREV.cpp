// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"

namespace frc {

TEST(DoubleSolenoidREVTest, ValidInitialization) {
  DoubleSolenoid solenoid{0, 3, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  solenoid.Set(DoubleSolenoid::kReverse);
  EXPECT_EQ(DoubleSolenoid::kReverse, solenoid.Get());

  solenoid.Set(DoubleSolenoid::kForward);
  EXPECT_EQ(DoubleSolenoid::kForward, solenoid.Get());

  solenoid.Set(DoubleSolenoid::kOff);
  EXPECT_EQ(DoubleSolenoid::kOff, solenoid.Get());
}

TEST(DoubleSolenoidREVTest, ThrowForwardPortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{0, 5, frc::PneumaticsModuleType::CTREPCM, 2};
  EXPECT_THROW(DoubleSolenoid(5, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidREVTest, ThrowReversePortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{0, 6, frc::PneumaticsModuleType::CTREPCM, 3};
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidREVTest, ThrowBothPortsAlreadyInitialized) {
  PneumaticsControlModule pcm{0, 6};
  // Single solenoid that is reused for forward port
  Solenoid solenoid0(0, 6, frc::PneumaticsModuleType::CTREPCM, 2);
  Solenoid solenoid1(0, 6, frc::PneumaticsModuleType::CTREPCM, 3);
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidREVTest, Toggle) {
  DoubleSolenoid solenoid{0, 4, frc::PneumaticsModuleType::CTREPCM, 2, 3};
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

TEST(DoubleSolenoidREVTest, InvalidForwardPort) {
  EXPECT_THROW(DoubleSolenoid(0, 0, frc::PneumaticsModuleType::CTREPCM, 100, 1),
               std::runtime_error);
}

TEST(DoubleSolenoidREVTest, InvalidReversePort) {
  EXPECT_THROW(DoubleSolenoid(0, 0, frc::PneumaticsModuleType::CTREPCM, 0, 100),
               std::runtime_error);
}
}  // namespace frc
