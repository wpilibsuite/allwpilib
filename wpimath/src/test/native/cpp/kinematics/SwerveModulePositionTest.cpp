// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveModulePosition.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SwerveModulePositionTest Equality", "[wpimath]") {
  wpi::math::SwerveModulePosition position1{2_m, 90_deg};
  wpi::math::SwerveModulePosition position2{2_m, 90_deg};

  CHECK(position1 == position2);
}

TEST_CASE("SwerveModulePositionTest Inequality", "[wpimath]") {
  wpi::math::SwerveModulePosition position1{1_m, 90_deg};
  wpi::math::SwerveModulePosition position2{2_m, 90_deg};
  wpi::math::SwerveModulePosition position3{1_m, 89_deg};

  CHECK(position1 != position2);
  CHECK(position1 != position3);
}
