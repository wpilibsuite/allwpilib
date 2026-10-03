// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cmath>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/sysid/analysis/AnalysisManager.hpp"
#include "wpi/sysid/analysis/FeedforwardAnalysis.hpp"
#include "wpi/sysid/analysis/FilteringUtils.hpp"
#include "wpi/sysid/analysis/Storage.hpp"

TEST_CASE("FilterTest MedianFilter", "[sysid]") {
  std::vector<sysid::PreparedData> testData{
      sysid::PreparedData{0_s, 0, 0, 0},    sysid::PreparedData{0_s, 0, 0, 1},
      sysid::PreparedData{0_s, 0, 0, 10},   sysid::PreparedData{0_s, 0, 0, 5},
      sysid::PreparedData{0_s, 0, 0, 3},    sysid::PreparedData{0_s, 0, 0, 0},
      sysid::PreparedData{0_s, 0, 0, 1000}, sysid::PreparedData{0_s, 0, 0, 7},
      sysid::PreparedData{0_s, 0, 0, 6},    sysid::PreparedData{0_s, 0, 0, 5}};
  std::vector<sysid::PreparedData> expectedData{
      sysid::PreparedData{0_s, 0, 0, 0}, sysid::PreparedData{0_s, 0, 0, 1},
      sysid::PreparedData{0_s, 0, 0, 5}, sysid::PreparedData{0_s, 0, 0, 5},
      sysid::PreparedData{0_s, 0, 0, 3}, sysid::PreparedData{0_s, 0, 0, 3},
      sysid::PreparedData{0_s, 0, 0, 7}, sysid::PreparedData{0_s, 0, 0, 7},
      sysid::PreparedData{0_s, 0, 0, 6}, sysid::PreparedData{0_s, 0, 0, 5}};

  sysid::ApplyMedianFilter(&testData, 3);
  CHECK(expectedData == testData);
}

TEST_CASE("FilterTest NoiseFloor", "[sysid]") {
  std::vector<sysid::PreparedData> testData = {
      {0_s, 1, 2, 3, 5_ms, 0, 0},    {1_s, 1, 2, 3, 5_ms, 1, 0},
      {2_s, 1, 2, 3, 5_ms, 2, 0},    {3_s, 1, 2, 3, 5_ms, 5, 0},
      {4_s, 1, 2, 3, 5_ms, 0.35, 0}, {5_s, 1, 2, 3, 5_ms, 0.15, 0},
      {6_s, 1, 2, 3, 5_ms, 0, 0},    {7_s, 1, 2, 3, 5_ms, 0.02, 0},
      {8_s, 1, 2, 3, 5_ms, 0.01, 0}, {9_s, 1, 2, 3, 5_ms, 0, 0}};
  double noiseFloor =
      GetNoiseFloor(testData, 2, [](auto&& pt) { return pt.acceleration; });
  CHECK(0.953 == Catch::Approx(noiseFloor).margin(0.001));
}

void FillStepVoltageData(std::vector<sysid::PreparedData>& data) {
  auto previousDatum = data.front();
  for (size_t i = 1; i < data.size(); ++i) {
    auto& datum = data.at(i);
    datum.timestamp = previousDatum.timestamp + previousDatum.dt;
    datum.position = 0.5 * previousDatum.acceleration *
                         wpi::units::math::pow<2>(previousDatum.dt).value() +
                     previousDatum.velocity * previousDatum.dt.value() +
                     previousDatum.position;
    datum.velocity = previousDatum.velocity +
                     previousDatum.acceleration * previousDatum.dt.value();

    previousDatum = datum;
  }
}

TEST_CASE("FilterTest StepTrim", "[sysid]") {
  {
    std::vector<sysid::PreparedData> forwardTestData = {
        {0_s, 1, 0, 0, 1_s, 0},  {0_s, 1, 0, 0, 1_s, 0.25},
        {0_s, 1, 0, 0, 1_s, 10}, {0_s, 1, 0, 0, 1_s, 0.45},
        {0_s, 1, 0, 0, 1_s, 0},  {0_s, 1, 0, 0, 1_s, 0.15},
        {0_s, 1, 0, 0, 1_s, 0},  {0_s, 1, 0, 0, 1_s, 0.02},
        {0_s, 1, 0, 0, 1_s, 0},  {0_s, 1, 0, 0, 0_s, 0},
    };

    FillStepVoltageData(forwardTestData);

    auto maxTime = 9_s;
    auto minTime = maxTime;

    sysid::AnalysisManager::Settings settings;
    auto [tempMinTime, positionDelay, velocityDelay] =
        sysid::TrimStepVoltageData(&forwardTestData, &settings, minTime,
                                   maxTime);
    minTime = tempMinTime;

    CHECK(3 == settings.stepTestDuration.value());
    CHECK(2 == minTime.value());
  }

  {
    std::vector<sysid::PreparedData> backwardsTestData = {
        {0_s, -1, 0, 0, 1_s, 0},     {0_s, -1, 0, 0, 1_s, -0.46},
        {0_s, -1, 0, 0, 1_s, -8},    {0_s, -1, 0, 0, 1_s, -0.32},
        {0_s, -1, 0, 0, 1_s, -0.12}, {0_s, -1, 0, 0, 1_s, -0.08},
        {0_s, -1, 0, 0, 1_s, -0.06}, {0_s, -1, 0, 0, 1_s, -0.02},
        {0_s, -1, 0, 0, 1_s, 0},     {0_s, -1, 0, 0, 0_s, 0},
    };

    FillStepVoltageData(backwardsTestData);

    auto maxTime = 9_s;
    auto minTime = maxTime;

    sysid::AnalysisManager::Settings settings;
    auto [tempMinTime, positionDelay, velocityDelay] =
        sysid::TrimStepVoltageData(&backwardsTestData, &settings, minTime,
                                   maxTime);
    minTime = tempMinTime;

    CHECK(3 == settings.stepTestDuration.value());
    CHECK(2 == minTime.value());
  }

  {
    // Forward test but with an erroneous negative acceleration at the end
    std::vector<sysid::PreparedData> noisyTestData = {
        {0_s, 1, 0, 0, 1_s, 0},    {0_s, 1, 0, 0, 1_s, 0.41},
        {0_s, 1, 0, 0, 1_s, 11.5}, {0_s, 1, 0, 0, 1_s, 1.2},
        {0_s, 1, 0, 0, 1_s, 0.34}, {0_s, 1, 0, 0, 1_s, 0.25},
        {0_s, 1, 0, 0, 1_s, 0.11}, {0_s, 1, 0, 0, 1_s, -0.08},
        {0_s, 1, 0, 0, 1_s, -12},  {0_s, 1, 0, 0, 0_s, 0},
    };

    FillStepVoltageData(noisyTestData);

    auto maxTime = 9_s;
    auto minTime = maxTime;

    sysid::AnalysisManager::Settings settings;
    auto [tempMinTime, positionDelay, velocityDelay] =
        sysid::TrimStepVoltageData(&noisyTestData, &settings, minTime, maxTime);
    minTime = tempMinTime;

    // Expect trimming to reject the erroneous peak negative accel,
    // correctly picking up the max positive accel instead.
    CHECK(4 == settings.stepTestDuration.value());
    CHECK(2 == minTime.value());
  }
}

template <int Derivative, int Samples, typename F, typename DfDx>
void AssertCentralResults(F&& f, DfDx&& dfdx, wpi::units::second_t h,
                          double min, double max) {
  static_assert(Samples % 2 != 0, "Number of samples must be odd.");

  auto filter = sysid::CentralFiniteDifference<Derivative, Samples>(h);

  for (int i = min / h.value(); i < max / h.value(); ++i) {
    // Let filter initialize
    if (i < static_cast<int>(min / h.value()) + Samples) {
      filter.Calculate(f(i * h.value()));
      continue;
    }

    // For central finite difference, the derivative computed at this point is
    // half the window size in the past.
    // The order of accuracy is O(h^(N - d)) where N is number of stencil
    // points and d is order of derivative
    CHECK(dfdx((i - static_cast<int>((Samples - 1) / 2)) * h.value()) ==
          Catch::Approx(filter.Calculate(f(i * h.value())))
              .margin(std::pow(h.value(), Samples - Derivative)));
  }
}

/**
 * Test central finite difference.
 */
TEST_CASE("LinearFilterOutputTest CentralFiniteDifference", "[sysid]") {
  constexpr auto h = 5_ms;

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = x²
        return x * x;
      },
      [](double x) {
        // df/dx = 2x
        return 2.0 * x;
      },
      h, -20.0, 20.0);

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = std::cos(x)
        return std::cos(x);
      },
      h, -20.0, 20.0);

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // df/dx = 1 / x
        return 1.0 / x;
      },
      h, 1.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = x^2
        return x * x;
      },
      [](double x) {
        // d²f/dx² = 2
        return 2.0;
      },
      h, -20.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // d²f/dx² = -std::sin(x)
        return -std::sin(x);
      },
      h, -20.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // d²f/dx² = -1 / x²
        return -1.0 / (x * x);
      },
      h, 1.0, 20.0);
}
