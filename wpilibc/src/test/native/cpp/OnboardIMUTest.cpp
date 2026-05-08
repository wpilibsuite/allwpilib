// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/imu/OnboardIMU.hpp"

#include <gtest/gtest.h>

#include "wpi/simulation/OnboardIMUSim.hpp"

using namespace wpi;

TEST(OnboardIMUTest, SimDevices) {
  OnboardIMU imu{OnboardIMU::FLAT};

  EXPECT_EQ(0.0, imu.GetAngleX().value());
  EXPECT_EQ(0.0, imu.GetAngleY().value());
  EXPECT_EQ(0.0, imu.GetAngleZ().value());

  EXPECT_EQ(0.0, imu.GetGyroRateX().value());
  EXPECT_EQ(0.0, imu.GetGyroRateY().value());
  EXPECT_EQ(0.0, imu.GetGyroRateZ().value());

  EXPECT_EQ(0.0, imu.GetAccelX().value());
  EXPECT_EQ(0.0, imu.GetAccelY().value());
  EXPECT_EQ(0.0, imu.GetAccelZ().value());

  EXPECT_EQ(0.0, imu.GetYaw().value());

  sim::OnboardIMUSim sim{};

  sim.SetAngleX(wpi::units::radian_t{1});
  sim.SetAngleY(wpi::units::radian_t{2});
  sim.SetAngleZ(wpi::units::radian_t{3});

  sim.SetGyroRateX(wpi::units::radians_per_second_t{3.504});
  sim.SetGyroRateY(wpi::units::radians_per_second_t{1.91});
  sim.SetGyroRateZ(wpi::units::radians_per_second_t{22.9});

  sim.SetAccelX(wpi::units::meters_per_second_squared_t{-1});
  sim.SetAccelY(wpi::units::meters_per_second_squared_t{-2});
  sim.SetAccelZ(wpi::units::meters_per_second_squared_t{-3});

  sim.SetYaw(wpi::units::radian_t{1.234});

  EXPECT_EQ(1.0, imu.GetAngleX().value());
  EXPECT_EQ(2.0, imu.GetAngleY().value());
  EXPECT_EQ(3.0, imu.GetAngleZ().value());

  EXPECT_EQ(3.504, imu.GetGyroRateX().value());
  EXPECT_EQ(1.91, imu.GetGyroRateY().value());
  EXPECT_EQ(22.9, imu.GetGyroRateZ().value());

  EXPECT_EQ(-1.0, imu.GetAccelX().value());
  EXPECT_EQ(-2.0, imu.GetAccelY().value());
  EXPECT_EQ(-3.0, imu.GetAccelZ().value());

  EXPECT_EQ(1.234, imu.GetYaw().value());
}
