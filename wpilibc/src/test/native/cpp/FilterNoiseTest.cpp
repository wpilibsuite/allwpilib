/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/filters/LinearDigitalFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <functional>
#include <memory>
#include <random>
#include <thread>

#include "frc/Base.h"
#include "gtest/gtest.h"

/* Filter constants */
static constexpr double kFilterStep = 0.005;
static constexpr double kFilterTime = 2.0;
static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
static constexpr double kSinglePoleIIRExpectedOutput = -3.2172003;
static constexpr double kHighPassTimeConstant = 0.006631;
static constexpr double kHighPassExpectedOutput = 10.074717;
static constexpr int32_t kMovAvgTaps = 6;
static constexpr double kMovAvgExpectedOutput = -10.191644;
static constexpr double kPi = 3.14159265358979323846;

using namespace frc;

enum FilterNoiseTestType { TEST_SINGLE_POLE_IIR, TEST_MOVAVG };

std::ostream& operator<<(std::ostream& os, const FilterNoiseTestType& type) {
  switch (type) {
    case TEST_SINGLE_POLE_IIR:
      os << "LinearDigitalFilter SinglePoleIIR";
      break;
    case TEST_MOVAVG:
      os << "LinearDigitalFilter MovingAverage";
      break;
  }

  return os;
}

constexpr double kStdDev = 10.0;

/**
 * Adds Gaussian white noise to a function returning data. The noise will have
 * the standard deviation provided in the constructor.
 */
class NoiseGenerator : public PIDSource {
 public:
  NoiseGenerator(double (*dataFunc)(double), double stdDev)
      : m_distr(0.0, stdDev) {
    m_dataFunc = dataFunc;
  }

  void SetPIDSourceType(PIDSourceType pidSource) override {}

  double Get() { return m_dataFunc(m_count) + m_noise; }

  double PIDGet() override {
    m_noise = m_distr(m_gen);
    m_count += kFilterStep;
    return m_dataFunc(m_count) + m_noise;
  }

  void Reset() { m_count = -kFilterStep; }

 private:
  std::function<double(double)> m_dataFunc;
  double m_noise = 0.0;

  // Make sure first call to PIDGet() uses m_count == 0
  double m_count = -kFilterStep;

  std::random_device m_rd;
  std::mt19937 m_gen{m_rd()};
  std::normal_distribution<double> m_distr;
};

/**
 * A fixture that includes a noise generator wrapped in a filter
 */
class FilterNoiseTest : public testing::TestWithParam<FilterNoiseTestType> {
 protected:
  std::unique_ptr<PIDSource> m_filter;
  std::shared_ptr<NoiseGenerator> m_noise;

  static double GetData(double t) { return 100.0 * std::sin(2.0 * kPi * t); }

  void SetUp() override {
    m_noise = std::make_shared<NoiseGenerator>(GetData, kStdDev);

    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::SinglePoleIIR(
                m_noise, kSinglePoleIIRTimeConstant, kFilterStep));
        break;
      }

      case TEST_MOVAVG: {
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::MovingAverage(m_noise, kMovAvgTaps));
        break;
      }
    }
  }
};

/**
 * Test if the filter reduces the noise produced by a signal generator
 */
TEST_P(FilterNoiseTest, NoiseReduce) {
  double theoryData = 0.0;
  double noiseGenError = 0.0;
  double filterError = 0.0;

  m_noise->Reset();
  for (double t = 0; t < kFilterTime; t += kFilterStep) {
    theoryData = GetData(t);
    filterError += std::abs(m_filter->PIDGet() - theoryData);
    noiseGenError += std::abs(m_noise->Get() - theoryData);
  }

  RecordProperty("FilterError", filterError);

  // The filter should have produced values closer to the theory
  EXPECT_GT(noiseGenError, filterError)
      << "Filter should have reduced noise accumulation but failed";
}

INSTANTIATE_TEST_CASE_P(Test, FilterNoiseTest,
                        testing::Values(TEST_SINGLE_POLE_IIR, TEST_MOVAVG), );
