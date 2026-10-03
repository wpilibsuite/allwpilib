// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"
#include "wpi/hardware/pneumatic/Solenoid.hpp"

namespace wpi {
TEST_CASE("SolenoidCTRETest ValidInitialization", "[wpilibc]") {
  Solenoid solenoid{CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 2};
  CHECK(2 == solenoid.GetChannel());

  solenoid.Set(true);
  CHECK(solenoid.Get());

  solenoid.Set(false);
  CHECK_FALSE(solenoid.Get());
}

TEST_CASE("SolenoidCTRETest DoubleInitialization", "[wpilibc]") {
  Solenoid solenoid{CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 2};
  CHECK_THROWS_AS(
      Solenoid(CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 2),
      std::runtime_error);
}

TEST_CASE("SolenoidCTRETest DoubleInitializationFromDoubleSolenoid",
          "[wpilibc]") {
  DoubleSolenoid solenoid{CANPort::CAN_S0, 3,
                          wpi::PneumaticsModuleType::CTRE_PCM, 2, 3};
  CHECK_THROWS_AS(
      Solenoid(CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 2),
      std::runtime_error);
}

TEST_CASE("SolenoidCTRETest InvalidChannel", "[wpilibc]") {
  CHECK_THROWS_AS(
      Solenoid(CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 100),
      std::runtime_error);
}

TEST_CASE("SolenoidCTRETest Toggle", "[wpilibc]") {
  Solenoid solenoid{CANPort::CAN_S0, 3, wpi::PneumaticsModuleType::CTRE_PCM, 2};
  solenoid.Set(true);
  CHECK(solenoid.Get());

  solenoid.Toggle();
  CHECK_FALSE(solenoid.Get());

  solenoid.Toggle();
  CHECK(solenoid.Get());
}
}  // namespace wpi
