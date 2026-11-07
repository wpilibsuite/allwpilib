// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/detail/Field2DView.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using wpi::glass::detail::FieldFrameData;
using wpi::math::Translation2d;

TEST_CASE("Field2D centered field coordinates", "[field2d]") {
  // Asymmetric image margins must not shift the field origin.
  FieldFrameData frame{{0, 0}, {1000, 600}, {100, 50}, {900, 450}, 50};
  auto center = frame.GetScreenFromPos({});
  CHECK(center.x == 500);
  CHECK(center.y == 250);
  CHECK(frame.GetPosFromScreen(center) == Translation2d{});

  auto blueCorner = frame.GetScreenFromPos({8_m, 4_m});
  CHECK(blueCorner.x == 900);
  CHECK(blueCorner.y == 50);
  CHECK(frame.GetPosFromScreen(blueCorner) == Translation2d{8_m, 4_m});
  auto redCorner = frame.GetScreenFromPos({-8_m, -4_m});
  CHECK(redCorner.x == 100);
  CHECK(redCorner.y == 450);
  CHECK(frame.GetPosFromScreen(redCorner) == Translation2d{-8_m, -4_m});

  CHECK(frame.GetPosFromScreen({0, 600}) == Translation2d{-8_m, -4_m});
  CHECK(frame.GetPosFromScreen({1000, 0}) == Translation2d{8_m, 4_m});
  Translation2d position{-2.5_m, 1.25_m};
  CHECK(frame.GetPosFromScreen(frame.GetScreenFromPos(position)) == position);
}

TEST_CASE("Field2D field headings", "[field2d]") {
  FieldFrameData frame{};
  auto zero = frame.GetScreenRotation(wpi::math::Rotation2d{0_deg});
  CHECK(zero.x == Catch::Approx(1));
  CHECK(zero.y == Catch::Approx(0).margin(1e-6));
  auto quarterTurn = frame.GetScreenRotation(wpi::math::Rotation2d{90_deg});
  CHECK(quarterTurn.x == Catch::Approx(0).margin(1e-6));
  CHECK(quarterTurn.y == Catch::Approx(-1));
  auto halfTurn = frame.GetScreenRotation(wpi::math::Rotation2d{180_deg});
  CHECK(halfTurn.x == Catch::Approx(-1));
  CHECK(halfTurn.y == Catch::Approx(0).margin(1e-6));
}
