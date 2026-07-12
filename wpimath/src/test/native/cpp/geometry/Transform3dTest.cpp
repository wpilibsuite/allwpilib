// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Transform3d.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"

using namespace wpi::math;

TEST_CASE("Transform3dTest ToMatrix", "[wpimath]") {
  Transform3d before{1_m, 2_m, 3_m, Rotation3d{10_deg, 20_deg, 30_deg}};
  Transform3d after{before.ToMatrix()};

  CHECK(before == after);
}

TEST_CASE("Transform3dTest Inverse", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 3_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform{{5_m, 4_m, 3_m}, Rotation3d{zAxis, 5_deg}};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  CHECK(initial == untransformed);
}

TEST_CASE("Transform3dTest Composition", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 3_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform1{{5_m, 0_m, 0_m}, Rotation3d{zAxis, 5_deg}};
  const Transform3d transform2{{0_m, 2_m, 0_m}, Rotation3d{zAxis, 5_deg}};

  auto transformedSeparate = initial + transform1 + transform2;
  auto transformedCombined = initial + (transform1 + transform2);

  CHECK(transformedSeparate == transformedCombined);
}
