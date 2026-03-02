// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <optional>
#include <string>

#include <gtest/gtest.h>

#include "frc/system/plant/DCMotor.h"

using namespace frc;

constexpr double kTolerance = 1e-12;

struct MotorTestParam {
  std::string name;
  std::function<DCMotor()> factory;
};

class DCMotorModelTest : public ::testing::TestWithParam<MotorTestParam> {
 protected:
  std::optional<DCMotor> motor;

  void SetUp() override { motor = GetParam().factory(); }
};

TEST_P(DCMotorModelTest, CurrentAtZeroSpeedAndNominalVoltageIsStallCurrent) {
  EXPECT_NEAR(motor->Current(0_rad_per_s, motor->nominalVoltage).value(),
              motor->stallCurrent.value(), kTolerance);
}

TEST_P(DCMotorModelTest, CurrentAtFreeSpeedAndNominalVoltageIsFreeCurrent) {
  EXPECT_NEAR(motor->Current(motor->freeSpeed, motor->nominalVoltage).value(),
              motor->freeCurrent.value(), kTolerance);
}

TEST_P(DCMotorModelTest, CurrentAtStallTorqueIsStallCurrent) {
  EXPECT_NEAR(motor->Current(motor->stallTorque).value(),
              motor->stallCurrent.value(), kTolerance);
}

TEST_P(DCMotorModelTest, CurrentAtZeroTorqueIsFreeCurrent) {
  EXPECT_NEAR(motor->Current(0.0_Nm).value(), motor->freeCurrent.value(),
              kTolerance);
}

TEST_P(DCMotorModelTest, TorqueAtFreeCurrentIsZero) {
  EXPECT_NEAR(motor->Torque(motor->freeCurrent).value(), 0.0, kTolerance);
}

TEST_P(DCMotorModelTest, TorqueAtStallCurrentIsStallTorque) {
  EXPECT_NEAR(motor->Torque(motor->stallCurrent).value(),
              motor->stallTorque.value(), kTolerance);
}

TEST_P(DCMotorModelTest, VoltageAtStallTorqueAndZeroSpeedIsNominalVoltage) {
  EXPECT_NEAR(motor->Voltage(motor->stallTorque, 0_rad_per_s).value(),
              motor->nominalVoltage.value(), kTolerance);
}

TEST_P(DCMotorModelTest, VoltageAtZeroTorqueAndFreeSpeedIsNominalVoltage) {
  EXPECT_NEAR(motor->Voltage(0.0_Nm, motor->freeSpeed).value(),
              motor->nominalVoltage.value(), kTolerance);
}

TEST_P(DCMotorModelTest, SpeedAtStallTorqueAndNominalVoltageIsZero) {
  EXPECT_NEAR(motor->Speed(motor->stallTorque, motor->nominalVoltage).value(),
              0.0, kTolerance);
}

TEST_P(DCMotorModelTest, SpeedAtZeroTorqueAndNominalVoltageIsFreeSpeed) {
  EXPECT_NEAR(motor->Speed(0.0_Nm, motor->nominalVoltage).value(),
              motor->freeSpeed.value(), kTolerance);
}

INSTANTIATE_TEST_SUITE_P(
    DCMotorTests, DCMotorModelTest,
    testing::Values(
        MotorTestParam{"CIM", []() { return DCMotor::CIM(); }},
        MotorTestParam{"MiniCIM", []() { return DCMotor::MiniCIM(); }},
        MotorTestParam{"Bag", []() { return DCMotor::Bag(); }},
        MotorTestParam{"Vex775Pro", []() { return DCMotor::Vex775Pro(); }},
        MotorTestParam{"RS775_125", []() { return DCMotor::RS775_125(); }},
        MotorTestParam{"BanebotsRS775",
                       []() { return DCMotor::BanebotsRS775(); }},
        MotorTestParam{"Andymark9015",
                       []() { return DCMotor::Andymark9015(); }},
        MotorTestParam{"BanebotsRS550",
                       []() { return DCMotor::BanebotsRS550(); }},
        MotorTestParam{"NEO", []() { return DCMotor::NEO(); }},
        MotorTestParam{"NEO550", []() { return DCMotor::NEO550(); }},
        MotorTestParam{"Falcon500", []() { return DCMotor::Falcon500(); }},
        MotorTestParam{"Falcon500FOC",
                       []() { return DCMotor::Falcon500FOC(); }},
        MotorTestParam{"RomiBuiltIn", []() { return DCMotor::RomiBuiltIn(); }},
        MotorTestParam{"KrakenX60", []() { return DCMotor::KrakenX60(); }},
        MotorTestParam{"KrakenX60FOC",
                       []() { return DCMotor::KrakenX60FOC(); }},
        MotorTestParam{"KrakenX44", []() { return DCMotor::KrakenX44(); }},
        MotorTestParam{"KrakenX44FOC",
                       []() { return DCMotor::KrakenX44FOC(); }},
        MotorTestParam{"Minion", []() { return DCMotor::Minion(); }},
        MotorTestParam{"NeoVortex", []() { return DCMotor::NeoVortex(); }}),
    [](const testing::TestParamInfo<MotorTestParam>& info) {
      return info.param.name;
    });
