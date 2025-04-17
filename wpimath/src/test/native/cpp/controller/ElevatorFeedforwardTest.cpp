// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/EigenCore.h"
#include "frc/controller/ElevatorFeedforward.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

TEST(ElevatorFeedforwardTest, CalculateVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V * 1_s / 1_m;
  constexpr auto Ka = 2_V * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_V;

  frc::ElevatorFeedforward<units::volt> elevatorFF{Ks, Kg, Kv, Ka};

  EXPECT_NEAR(elevatorFF.Calculate(0_m / 1_s).value(), Kg.value(), 0.002);
  EXPECT_NEAR(elevatorFF.Calculate(2_m / 1_s).value(), 4.5, 0.002);

  frc::Matrixd<1, 1> A{-Kv.value() / Ka.value()};
  frc::Matrixd<1, 1> B{1.0 / Ka.value()};
  constexpr units::second_t dt = 20_ms;
  frc::LinearPlantInversionFeedforward<1, 1> plantInversion{A, B, dt};

  frc::Vectord<1> r{2.0};
  frc::Vectord<1> nextR{3.0};
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + Ks.value() + Kg.value(),
              elevatorFF.Calculate(2_mps, 3_mps).value(), 0.002);
}

TEST(ElevatorFeedforwardTest, CalculateCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A * 1_s / 1_m;
  constexpr auto Ka = 2_A * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_A;

  frc::ElevatorFeedforward<units::ampere> elevatorFF{Ks, Kg, Kv, Ka};

  EXPECT_NEAR(elevatorFF.Calculate(0_m / 1_s).value(), Kg.value(), 0.002);
  EXPECT_NEAR(elevatorFF.Calculate(2_m / 1_s).value(), 4.5, 0.002);

  frc::Matrixd<1, 1> A{-Kv.value() / Ka.value()};
  frc::Matrixd<1, 1> B{1.0 / Ka.value()};
  constexpr units::second_t dt = 20_ms;
  frc::LinearPlantInversionFeedforward<1, 1> plantInversion{A, B, dt};

  frc::Vectord<1> r{2.0};
  frc::Vectord<1> nextR{3.0};
  EXPECT_NEAR(plantInversion.Calculate(r, nextR)(0) + Ks.value() + Kg.value(),
              elevatorFF.Calculate(2_mps, 3_mps).value(), 0.002);
}

TEST(ElevatorFeedforwardTest, AchievableVelocityVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V * 1_s / 1_m;
  constexpr auto Ka = 2_V * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_V;

  frc::ElevatorFeedforward<units::volt> elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.MaxAchievableVelocity(11_V, 1_m / 1_s / 1_s).value(),
              5, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableVelocity(11_V, 1_m / 1_s / 1_s).value(),
              -9, 0.002);
}

TEST(ElevatorFeedforwardTest, AchievableVelocityCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A * 1_s / 1_m;
  constexpr auto Ka = 2_A * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_A;

  frc::ElevatorFeedforward<units::ampere> elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.MaxAchievableVelocity(11_A, 1_m / 1_s / 1_s).value(),
              5, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableVelocity(11_A, 1_m / 1_s / 1_s).value(),
              -9, 0.002);
}

TEST(ElevatorFeedforwardTest, AchievableAccelerationVoltage) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V * 1_s / 1_m;
  constexpr auto Ka = 2_V * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_V;

  frc::ElevatorFeedforward<units::volt> elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_V, 2_m / 1_s).value(),
              3.75, 0.002);
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_V, -2_m / 1_s).value(),
              7.25, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_V, 2_m / 1_s).value(),
              -8.25, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_V, -2_m / 1_s).value(),
              -4.75, 0.002);
}

TEST(ElevatorFeedforwardTest, AchievableAccelerationCurrent) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A * 1_s / 1_m;
  constexpr auto Ka = 2_A * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_A;

  frc::ElevatorFeedforward<units::ampere> elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_A, 2_m / 1_s).value(),
              3.75, 0.002);
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_A, -2_m / 1_s).value(),
              7.25, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_A, 2_m / 1_s).value(),
              -8.25, 0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_A, -2_m / 1_s).value(),
              -4.75, 0.002);
}

TEST(ElevatorFeedforwardTest, NegativeVoltageGains) {
  constexpr auto Ks = 0.5_V;
  constexpr auto Kv = 1.5_V * 1_s / 1_m;
  constexpr auto Ka = 2_V * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_V;

  frc::ElevatorFeedforward<units::volt> elevatorFF{Ks, Kg, -Kv, -Ka};
  EXPECT_EQ(elevatorFF.GetKv().value(), 0);
  EXPECT_EQ(elevatorFF.GetKa().value(), 0);
}

TEST(ElevatorFeedforwardTest, NegativeCurrentGains) {
  constexpr auto Ks = 0.5_A;
  constexpr auto Kv = 1.5_A * 1_s / 1_m;
  constexpr auto Ka = 2_A * 1_s * 1_s / 1_m;
  constexpr auto Kg = 1_A;

  frc::ElevatorFeedforward<units::ampere> elevatorFF{Ks, Kg, -Kv, -Ka};
  EXPECT_EQ(elevatorFF.GetKv().value(), 0);
  EXPECT_EQ(elevatorFF.GetKa().value(), 0);
}
