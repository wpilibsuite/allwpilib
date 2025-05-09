// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"

namespace frc {

TEST(DoubleSolenoidCTRETest, ValidInitialization) {
  DoubleSolenoid solenoid{0, 3, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  solenoid.Set(DoubleSolenoid::REVERSE);
  EXPECT_EQ(DoubleSolenoid::REVERSE, solenoid.Get());

  solenoid.Set(DoubleSolenoid::FORWARD);
  EXPECT_EQ(DoubleSolenoid::FORWARD, solenoid.Get());

  solenoid.Set(DoubleSolenoid::OFF);
  EXPECT_EQ(DoubleSolenoid::OFF, solenoid.Get());
}

TEST(DoubleSolenoidCTRETest, ThrowForwardPortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{0, 5, frc::PneumaticsModuleType::CTREPCM, 2};
  EXPECT_THROW(DoubleSolenoid(0, 5, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, ThrowReversePortAlreadyInitialized) {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{0, 6, frc::PneumaticsModuleType::CTREPCM, 3};
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, ThrowBothPortsAlreadyInitialized) {
  PneumaticsControlModule pcm{0, 6};
  // Single solenoid that is reused for forward port
  Solenoid solenoid0(0, 6, frc::PneumaticsModuleType::CTREPCM, 2);
  Solenoid solenoid1(0, 6, frc::PneumaticsModuleType::CTREPCM, 3);
  EXPECT_THROW(DoubleSolenoid(6, frc::PneumaticsModuleType::CTREPCM, 2, 3),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, Toggle) {
  DoubleSolenoid solenoid{0, 4, frc::PneumaticsModuleType::CTREPCM, 2, 3};
  // Bootstrap it into reverse
  solenoid.Set(DoubleSolenoid::REVERSE);

  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::FORWARD, solenoid.Get());

  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::REVERSE, solenoid.Get());

  // Of shouldn't do anything on toggle
  solenoid.Set(DoubleSolenoid::OFF);
  solenoid.Toggle();
  EXPECT_EQ(DoubleSolenoid::OFF, solenoid.Get());
}

TEST(DoubleSolenoidCTRETest, InvalidForwardPort) {
  EXPECT_THROW(DoubleSolenoid(0, 0, frc::PneumaticsModuleType::CTREPCM, 100, 1),
               std::runtime_error);
}

TEST(DoubleSolenoidCTRETest, InvalidReversePort) {
  EXPECT_THROW(DoubleSolenoid(0, 0, frc::PneumaticsModuleType::CTREPCM, 0, 100),
               std::runtime_error);
}
}  // namespace frc
