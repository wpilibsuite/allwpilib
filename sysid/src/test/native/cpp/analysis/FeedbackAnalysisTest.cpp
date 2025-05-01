// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "sysid/analysis/FeedbackAnalysis.h"
#include "sysid/analysis/FeedbackControllerPreset.h"

TEST(FeedbackAnalysisTest, VelocitySystem1) {
  auto V = 3.060;
  auto A = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::DEFAULT, params, V, A);

  EXPECT_NEAR(Kp, 2.11, 0.05);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocitySystem2) {
  auto V = 0.0693;
  auto A = 0.1170;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::DEFAULT, params, V, A);

  EXPECT_NEAR(Kp, 3.11, 0.05);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocitySystemWithSmallKa) {
  auto V = 3.060;
  auto A = 0.0;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::DEFAULT, params, V, A);

  EXPECT_NEAR(Kp, 0.00, 0.05);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocityConversion) {
  auto V = 0.0693;
  auto A = 0.1170;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::DEFAULT, params, V, A, 3.0 * 1023);

  // This should have the same Kp as the test above, but scaled by a factor of 3
  // * 1023.
  EXPECT_NEAR(Kp, 3.11 / (3 * 1023), 0.005);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocityCTRE) {
  auto V = 1.97;
  auto A = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(sysid::presets::CTRE_V5,
                                                        params, V, A);

  EXPECT_NEAR(Kp, 259.21276731541178, 0.00005);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocityCTREConversion) {
  auto V = 1.97;
  auto A = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(sysid::presets::CTRE_V5,
                                                        params, V, A, 3.0);

  // This should have the same Kp as the test above, but scaled by a factor
  // of 3.
  EXPECT_NEAR(Kp, 259.21276731541178 / 3, 0.00005);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocityREV) {
  auto V = 1.97;
  auto A = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::REV_NEO_BUILTIN, params, V, A);

  EXPECT_NEAR(Kp, 0.00241, 0.005);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, VelocityREVConversion) {
  auto V = 1.97;
  auto A = 0.179;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculateVelocityFeedbackGains(
      sysid::presets::REV_NEO_BUILTIN, params, V, A, 3.0);

  // This should have the same Kp as the test above, but scaled by a factor
  // of 3.
  EXPECT_NEAR(Kp, 0.00241 / 3, 0.005);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, Position) {
  auto V = 3.060;
  auto A = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::DEFAULT, params, V, A);

  EXPECT_NEAR(Kp, 6.41, 0.05);
  EXPECT_NEAR(Kd, 2.48, 0.05);
}

TEST(FeedbackAnalysisTest, PositionWithSmallKa) {
  auto V = 3.060;
  auto A = 1e-10;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::DEFAULT, params, V, A);

  EXPECT_NEAR(Kp, 19.97, 0.05);
  EXPECT_NEAR(Kd, 0.00, 0.05);
}

TEST(FeedbackAnalysisTest, PositionWithLatencyCompensation) {
  auto V = 3.060;
  auto A = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};
  sysid::FeedbackControllerPreset preset{sysid::presets::DEFAULT};

  preset.measurementDelay = 10_ms;
  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(preset, params, V, A);

  EXPECT_NEAR(Kp, 5.92, 0.05);
  EXPECT_NEAR(Kd, 2.12, 0.05);
}

TEST(FeedbackAnalysisTest, PositionREV) {
  auto V = 3.060;
  auto A = 0.327;

  sysid::LQRParameters params{1, 1.5, 7};

  auto [Kp, Kd] = sysid::CalculatePositionFeedbackGains(
      sysid::presets::REV_NEO_BUILTIN, params, V, A);

  EXPECT_NEAR(Kp, 0.30202, 0.05);
  EXPECT_NEAR(Kd, 48.518, 0.05);
}
