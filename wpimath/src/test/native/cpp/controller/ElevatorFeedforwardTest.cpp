// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/controller/ElevatorFeedforward.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

namespace frc {

TEST(ElevatorFeedforwardTest, Calculate) {
  auto Ks = 0.5_V;
  auto Kv = 1.5_V * 1_s / 1_m;
  auto Ka = 2_V * 1_s * 1_s / 1_m;
  auto Kg = 1_V;


  frc::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.Calculate(0_m/1_s).value(),Kg.value(),0.002);
  EXPECT_NEAR(elevatorFF.Calculate(2_m/1_s).value(),4.5,0.002);
  EXPECT_NEAR(elevatorFF.Calculate(2_m/1_s, 1_m/1_s/1_s).value(),6.5,0.002);
  EXPECT_NEAR(elevatorFF.Calculate(-2_m/1_s, 1_m/1_s/1_s).value(),-0.5,0.002);
}

TEST(ElevatorFeedforwardTest, AchievableVelocity) {
  auto Ks = 0.5_V;
  auto Kv = 1.5_V * 1_s / 1_m;
  auto Ka = 2_V * 1_s * 1_s / 1_m;
  auto Kg = 1_V;

  frc::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(elevatorFF.MaxAchievableVelocity(11_V, 1_m/1_s/1_s).value(),5,0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableVelocity(11_V, 1_m/1_s/1_s).value(),-9,0.002);
}


TEST(ElevatorFeedforwardTest, AchievableAcceleration) {
  auto Ks = 0.5_V;
  auto Kv = 1.5_V * 1_s / 1_m;
  auto Ka = 2_V * 1_s * 1_s / 1_m;
  auto Kg = 1_V;

  frc::ElevatorFeedforward elevatorFF{Ks, Kg, Kv, Ka}; // 
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_V, 2_m/1_s).value(),3.75,0.002);
  EXPECT_NEAR(elevatorFF.MaxAchievableAcceleration(12_V, -2_m/1_s).value(),7.25,0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_V, 2_m/1_s).value(),-8.25,0.002);
  EXPECT_NEAR(elevatorFF.MinAchievableAcceleration(12_V, -2_m/1_s).value(),-4.75,0.002);
}

}  // namespace frc
