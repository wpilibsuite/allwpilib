// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/imu/OnboardIMU.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/OnboardIMUSim.hpp"

using namespace wpi;

namespace {
struct OnboardIMUTest {
  OnboardIMUTest() { ResetData(); }
  ~OnboardIMUTest() { ResetData(); }
  void ResetData() {
    sim::OnboardIMUSim sim;
    sim.SetAngleX(0_rad);
    sim.SetAngleY(0_rad);
    sim.SetAngleZ(0_rad);
    sim.SetGyroRateX(0_rad_per_s);
    sim.SetGyroRateY(0_rad_per_s);
    sim.SetGyroRateZ(0_rad_per_s);
    sim.SetAccelX(0_mps_sq);
    sim.SetAccelY(0_mps_sq);
    sim.SetAccelZ(0_mps_sq);
    sim.SetYaw(0_rad);
  }
};
}  // namespace

TEST_CASE_METHOD(OnboardIMUTest, "OnboardIMUTest SimDevices", "[wpilibc]") {
  OnboardIMU imu{OnboardIMU::FLAT};

  CHECK(0.0 == imu.GetAngleX().value());
  CHECK(0.0 == imu.GetAngleY().value());
  CHECK(0.0 == imu.GetAngleZ().value());

  CHECK(0.0 == imu.GetGyroRateX().value());
  CHECK(0.0 == imu.GetGyroRateY().value());
  CHECK(0.0 == imu.GetGyroRateZ().value());

  CHECK(0.0 == imu.GetAccelX().value());
  CHECK(0.0 == imu.GetAccelY().value());
  CHECK(0.0 == imu.GetAccelZ().value());

  CHECK(0.0 == imu.GetYaw().value());
  CHECK(imu.GetQuaternion() == wpi::math::Quaternion{});

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

  CHECK(1.0 == imu.GetAngleX().value());
  CHECK(2.0 == imu.GetAngleY().value());
  CHECK(3.0 == imu.GetAngleZ().value());

  CHECK(3.504 == imu.GetGyroRateX().value());
  CHECK(1.91 == imu.GetGyroRateY().value());
  CHECK(22.9 == imu.GetGyroRateZ().value());

  CHECK(-1.0 == imu.GetAccelX().value());
  CHECK(-2.0 == imu.GetAccelY().value());
  CHECK(-3.0 == imu.GetAccelZ().value());

  CHECK(1.234 == imu.GetYaw().value());
  auto rotation = wpi::math::Rotation3d{1_rad, 2_rad, 3_rad};
  CHECK(imu.GetRotation3d() == rotation);
  imu.ResetYaw();
  CHECK(imu.GetYaw() == 0_rad);
  sim.SetYaw(2_rad);
  CHECK_THAT(imu.GetRotation2d().Radians().value(),
             Catch::Matchers::WithinAbs(0.766, 1e-9));
  CHECK(imu.GetAngleZ() == 3_rad);
  CHECK(imu.GetRotation3d() == rotation);
}
