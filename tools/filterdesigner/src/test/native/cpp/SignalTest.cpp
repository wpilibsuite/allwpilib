// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Signal.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace {

using wpi::filterdesigner::Signal;

TEST(SignalTest, InferSampleRateEmpty) {
  std::vector<double> ts;
  EXPECT_EQ(Signal::InferSampleRate(ts), 0.0);
}

TEST(SignalTest, InferSampleRateSingleSample) {
  std::vector<double> ts{0.1};
  EXPECT_EQ(Signal::InferSampleRate(ts), 0.0);
}

TEST(SignalTest, InferSampleRateUniform1kHz) {
  std::vector<double> ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(i * 0.001);
  }
  EXPECT_NEAR(Signal::InferSampleRate(ts), 1000.0, 1e-9);
}

TEST(SignalTest, InferSampleRateRobustToOutlierGap) {
  // 50 Hz sampling with one 10x gap in the middle; median should survive.
  std::vector<double> ts;
  for (int i = 0; i < 20; ++i) {
    ts.push_back(i * 0.02);
  }
  ts.push_back(ts.back() + 0.2);  // big gap
  for (int i = 0; i < 20; ++i) {
    ts.push_back(ts.back() + 0.02);
  }
  EXPECT_NEAR(Signal::InferSampleRate(ts), 50.0, 1e-9);
}

TEST(SignalTest, InferSampleRateZeroForNonPositivePeriod) {
  std::vector<double> ts{0.0, 0.0, 0.0, 0.0};
  EXPECT_EQ(Signal::InferSampleRate(ts), 0.0);
}

TEST(SignalTest, IsUniformTrueForEvenlySpaced) {
  std::vector<double> ts;
  for (int i = 0; i < 50; ++i) {
    ts.push_back(i * 0.01);
  }
  EXPECT_TRUE(Signal::IsUniform(ts));
}

TEST(SignalTest, IsUniformFalseWithOneOffGap) {
  std::vector<double> ts;
  for (int i = 0; i < 10; ++i) {
    ts.push_back(i * 0.01);
  }
  ts.push_back(ts.back() + 0.05);
  EXPECT_FALSE(Signal::IsUniform(ts));
}

TEST(SignalTest, IsUniformFalseForEmptyOrSingle) {
  std::vector<double> empty;
  EXPECT_FALSE(Signal::IsUniform(empty));
  std::vector<double> one{1.0};
  EXPECT_FALSE(Signal::IsUniform(one));
}

TEST(SignalTest, IsUniformToleranceRespected) {
  std::vector<double> ts{0.0, 0.01, 0.02000001, 0.03};
  EXPECT_TRUE(Signal::IsUniform(ts, 1e-4));
  EXPECT_FALSE(Signal::IsUniform(ts, 1e-9));
}

}  // namespace
