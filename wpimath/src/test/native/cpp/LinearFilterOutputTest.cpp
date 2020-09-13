/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <functional>
#include <memory>
#include <random>

#include <wpi/math>

#include "gtest/gtest.h"
#include "units/time.h"

// Filter constants
static constexpr units::second_t kFilterStep = 0.005_s;
static constexpr units::second_t kFilterTime = 2.0_s;
static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
static constexpr double kSinglePoleIIRExpectedOutput = -3.2172003;
static constexpr double kHighPassTimeConstant = 0.006631;
static constexpr double kHighPassExpectedOutput = 10.074717;
static constexpr int32_t kMovAvgTaps = 6;
static constexpr double kMovAvgExpectedOutput = -10.191644;

enum LinearFilterOutputTestType {
  TEST_SINGLE_POLE_IIR,
  TEST_HIGH_PASS,
  TEST_MOVAVG,
  TEST_PULSE
};

std::ostream& operator<<(std::ostream& os,
                         const LinearFilterOutputTestType& type) {
  switch (type) {
    case TEST_SINGLE_POLE_IIR:
      os << "LinearFilter SinglePoleIIR";
      break;
    case TEST_HIGH_PASS:
      os << "LinearFilter HighPass";
      break;
    case TEST_MOVAVG:
      os << "LinearFilter MovingAverage";
      break;
    case TEST_PULSE:
      os << "LinearFilter Pulse";
      break;
  }

  return os;
}

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * wpi::math::pi * t) +
         20.0 * std::cos(50.0 * wpi::math::pi * t);
}

static double GetPulseData(double t) {
  if (std::abs(t - 1.0) < 0.001) {
    return 1.0;
  } else {
    return 0.0;
  }
}

/**
 * A fixture that includes a consistent data source wrapped in a filter
 */
class LinearFilterOutputTest
    : public testing::TestWithParam<LinearFilterOutputTestType> {
 protected:
  std::unique_ptr<frc::LinearFilter<double>> m_filter;
  std::function<double(double)> m_data;
  double m_expectedOutput = 0.0;

  void SetUp() override {
    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::SinglePoleIIR(kSinglePoleIIRTimeConstant,
                                                     kFilterStep));
        m_data = GetData;
        m_expectedOutput = kSinglePoleIIRExpectedOutput;
        break;
      }

      case TEST_HIGH_PASS: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::HighPass(kHighPassTimeConstant,
                                                kFilterStep));
        m_data = GetData;
        m_expectedOutput = kHighPassExpectedOutput;
        break;
      }

      case TEST_MOVAVG: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::MovingAverage(kMovAvgTaps));
        m_data = GetData;
        m_expectedOutput = kMovAvgExpectedOutput;
        break;
      }

      case TEST_PULSE: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::MovingAverage(kMovAvgTaps));
        m_data = GetPulseData;
        m_expectedOutput = 0.0;
        break;
      }
    }
  }
};

/**
 * Test if the linear filters produce consistent output for a given data set.
 */
TEST_P(LinearFilterOutputTest, Output) {
  double filterOutput = 0.0;
  for (auto t = 0_s; t < kFilterTime; t += kFilterStep) {
    filterOutput = m_filter->Calculate(m_data(t.to<double>()));
  }

  RecordProperty("LinearFilterOutput", filterOutput);

  EXPECT_FLOAT_EQ(m_expectedOutput, filterOutput)
      << "Filter output didn't match expected value";
}

INSTANTIATE_TEST_SUITE_P(Test, LinearFilterOutputTest,
                         testing::Values(TEST_SINGLE_POLE_IIR, TEST_HIGH_PASS,
                                         TEST_MOVAVG, TEST_PULSE));
