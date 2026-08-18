// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/FeedbackAnalysis.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/sysid/analysis/FeedbackControllerPreset.hpp"

TEST_CASE("FeedbackAnalysisTest VelocitySystem1", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(2.11).margin(0.05));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocitySystem2", "[sysid]") {
  auto Kv = 0.0693;
  auto Ka = 0.1170;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(3.11).margin(0.05));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocitySystemWithSmallKa", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 0.0;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(0.00).margin(0.05));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocityConversion", "[sysid]") {
  auto Kv = 0.0693;
  auto Ka = 0.1170;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka, 3.0 * 1023);

  // This should have the same Kp as the test above, but scaled by a factor of 3
  // * 1023.
  CHECK(Kp == Catch::Approx(3.11 / (3 * 1023)).margin(0.005));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocityCTRE", "[sysid]") {
  auto Kv = 1.97;
  auto Ka = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(sysid::presets::kCTREv5,
                                                        params, Kv, Ka);

  CHECK(Kp == Catch::Approx(259.21276731541178).margin(0.00005));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocityCTREConversion", "[sysid]") {
  auto Kv = 1.97;
  auto Ka = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(sysid::presets::kCTREv5,
                                                        params, Kv, Ka, 3.0);

  // This should have the same Kp as the test above, but scaled by a factor
  // of 3.
  CHECK(Kp == Catch::Approx(259.21276731541178 / 3).margin(0.00005));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocityREV", "[sysid]") {
  auto Kv = 1.97;
  auto Ka = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kREVNEOBuiltIn, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(0.00241).margin(0.005));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest VelocityREVConversion", "[sysid]") {
  auto Kv = 1.97;
  auto Ka = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::kREVNEOBuiltIn, params, Kv, Ka, 3.0);

  // This should have the same Kp as the test above, but scaled by a factor
  // of 3.
  CHECK(Kp == Catch::Approx(0.00241 / 3).margin(0.005));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest Position", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(6.41).margin(0.05));
  CHECK(Kd == Catch::Approx(2.48).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest PositionWithSmallKa", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 1e-10;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::kDefault, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(19.97).margin(0.05));
  CHECK(Kd == Catch::Approx(0.00).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest PositionWithLatencyCompensation", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};
  sysid::FeedbackControllerPreset preset{sysid::presets::kDefault};

  preset.measurementDelay = 10_ms;
  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(preset, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(5.92).margin(0.05));
  CHECK(Kd == Catch::Approx(2.12).margin(0.05));
}

TEST_CASE("FeedbackAnalysisTest PositionREV", "[sysid]") {
  auto Kv = 3.060;
  auto Ka = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::kREVNEOBuiltIn, params, Kv, Ka);

  CHECK(Kp == Catch::Approx(0.30202).margin(0.05));
  CHECK(Kd == Catch::Approx(48.518).margin(0.05));
}
