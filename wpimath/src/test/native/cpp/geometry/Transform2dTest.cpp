// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Transform2d.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"

using namespace wpi::math;

TEST_CASE("Transform2dTest ToMatrix", "[wpimath]") {
  Transform2d before{1_m, 2_m, 20_deg};
  Transform2d after{before.ToMatrix()};

  CHECK(before == after);
}

TEST_CASE("Transform2dTest Inverse", "[wpimath]") {
  const Pose2d initial{1_m, 2_m, 45_deg};
  const Transform2d transform{{5_m, 0_m}, 5_deg};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  CHECK(initial == untransformed);
}

TEST_CASE("Transform2dTest Composition", "[wpimath]") {
  const Pose2d initial{1_m, 2_m, 45_deg};
  const Transform2d transform1{{5_m, 0_m}, 5_deg};
  const Transform2d transform2{{0_m, 2_m}, 5_deg};

  auto transformedSeparate = initial + transform1 + transform2;
  auto transformedCombined = initial + (transform1 + transform2);

  CHECK(transformedSeparate == transformedCombined);
}

TEST_CASE("Transform2dTest Constexpr", "[wpimath]") {
  constexpr Transform2d defaultCtor;
  constexpr Transform2d translationRotationCtor{Translation2d{},
                                                Rotation2d{10_deg}};
  constexpr auto multiplied = translationRotationCtor * 5;
  constexpr auto divided = translationRotationCtor / 2;

  static_assert(defaultCtor.Translation().X() == 0_m);
  static_assert(translationRotationCtor.X() == 0_m);
  static_assert(translationRotationCtor.Y() == 0_m);
  static_assert(multiplied.Rotation().Degrees() == 50_deg);
  static_assert(translationRotationCtor.Inverse().Rotation().Degrees() ==
                (-10_deg));
  static_assert(translationRotationCtor.Inverse().X() == 0_m);
  static_assert(translationRotationCtor.Inverse().Y() == 0_m);
  static_assert(divided.Rotation().Degrees() == 5_deg);
}
