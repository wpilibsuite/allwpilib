// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"
#include "wpi/hardware/pneumatic/PneumaticsControlModule.hpp"
#include "wpi/hardware/pneumatic/Solenoid.hpp"

namespace wpi {

TEST_CASE("DoubleSolenoidCTRETest ValidInitialization", "[wpilibc]") {
  DoubleSolenoid solenoid{CANPort::CAN_S0, 3,
                          wpi::PneumaticsModuleType::CTRE_PCM, 2, 3};
  solenoid.Set(DoubleSolenoid::REVERSE);
  CHECK(DoubleSolenoid::REVERSE == solenoid.Get());

  solenoid.Set(DoubleSolenoid::FORWARD);
  CHECK(DoubleSolenoid::FORWARD == solenoid.Get());

  solenoid.Set(DoubleSolenoid::OFF);
  CHECK(DoubleSolenoid::OFF == solenoid.Get());
}

TEST_CASE("DoubleSolenoidCTRETest ThrowForwardPortAlreadyInitialized",
          "[wpilibc]") {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{CANPort::CAN_S0, 5, wpi::PneumaticsModuleType::CTRE_PCM, 2};
  CHECK_THROWS_AS(DoubleSolenoid(CANPort::CAN_S0, 5,
                                 wpi::PneumaticsModuleType::CTRE_PCM, 2, 3),
                  std::runtime_error);
}

TEST_CASE("DoubleSolenoidCTRETest ThrowReversePortAlreadyInitialized",
          "[wpilibc]") {
  // Single solenoid that is reused for forward port
  Solenoid solenoid{CANPort::CAN_S0, 6, wpi::PneumaticsModuleType::CTRE_PCM, 3};
  CHECK_THROWS_AS(DoubleSolenoid(CANPort::CAN_S0, 6,
                                 wpi::PneumaticsModuleType::CTRE_PCM, 2, 3),
                  std::runtime_error);
}

TEST_CASE("DoubleSolenoidCTRETest ThrowBothPortsAlreadyInitialized",
          "[wpilibc]") {
  PneumaticsControlModule pcm{CANPort::CAN_S0, 6};
  // Single solenoid that is reused for forward port
  Solenoid solenoid0(CANPort::CAN_S0, 6, wpi::PneumaticsModuleType::CTRE_PCM,
                     2);
  Solenoid solenoid1(CANPort::CAN_S0, 6, wpi::PneumaticsModuleType::CTRE_PCM,
                     3);
  CHECK_THROWS_AS(DoubleSolenoid(CANPort::CAN_S0, 6,
                                 wpi::PneumaticsModuleType::CTRE_PCM, 2, 3),
                  std::runtime_error);
}

TEST_CASE("DoubleSolenoidCTRETest Toggle", "[wpilibc]") {
  DoubleSolenoid solenoid{CANPort::CAN_S0, 4,
                          wpi::PneumaticsModuleType::CTRE_PCM, 2, 3};
  // Bootstrap it into reverse
  solenoid.Set(DoubleSolenoid::REVERSE);

  solenoid.Toggle();
  CHECK(DoubleSolenoid::FORWARD == solenoid.Get());

  solenoid.Toggle();
  CHECK(DoubleSolenoid::REVERSE == solenoid.Get());

  // Of shouldn't do anything on toggle
  solenoid.Set(DoubleSolenoid::OFF);
  solenoid.Toggle();
  CHECK(DoubleSolenoid::OFF == solenoid.Get());
}

TEST_CASE("DoubleSolenoidCTRETest InvalidForwardPort", "[wpilibc]") {
  CHECK_THROWS_AS(DoubleSolenoid(CANPort::CAN_S0, 0,
                                 wpi::PneumaticsModuleType::CTRE_PCM, 100, 1),
                  std::runtime_error);
}

TEST_CASE("DoubleSolenoidCTRETest InvalidReversePort", "[wpilibc]") {
  CHECK_THROWS_AS(DoubleSolenoid(CANPort::CAN_S0, 0,
                                 wpi::PneumaticsModuleType::CTRE_PCM, 0, 100),
                  std::runtime_error);
}
}  // namespace wpi
