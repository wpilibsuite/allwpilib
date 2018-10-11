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

enum FilterOutputTestType {
  TEST_SINGLE_POLE_IIR,
  TEST_HIGH_PASS,
  TEST_MOVAVG,
  TEST_PULSE
};

std::ostream& operator<<(std::ostream& os, const FilterOutputTestType& type) {
  switch (type) {
    case TEST_SINGLE_POLE_IIR:
      os << "LinearDigitalFilter SinglePoleIIR";
      break;
    case TEST_HIGH_PASS:
      os << "LinearDigitalFilter HighPass";
      break;
    case TEST_MOVAVG:
      os << "LinearDigitalFilter MovingAverage";
      break;
    case TEST_PULSE:
      os << "LinearDigitalFilter Pulse";
      break;
  }

  return os;
}

class DataWrapper : public PIDSource {
 public:
  explicit DataWrapper(double (*dataFunc)(double)) { m_dataFunc = dataFunc; }

  virtual void SetPIDSourceType(PIDSourceType pidSource) {}

  virtual double PIDGet() {
    m_count += kFilterStep;
    return m_dataFunc(m_count);
  }

  void Reset() { m_count = -kFilterStep; }

 private:
  std::function<double(double)> m_dataFunc;

  // Make sure first call to PIDGet() uses m_count == 0
  double m_count = -kFilterStep;
};

/**
 * A fixture that includes a consistent data source wrapped in a filter
 */
class FilterOutputTest : public testing::TestWithParam<FilterOutputTestType> {
 protected:
  std::unique_ptr<PIDSource> m_filter;
  std::shared_ptr<DataWrapper> m_data;
  double m_expectedOutput = 0.0;

  static double GetData(double t) {
    return 100.0 * std::sin(2.0 * kPi * t) + 20.0 * std::cos(50.0 * kPi * t);
  }

  static double GetPulseData(double t) {
    if (std::abs(t - 1.0) < 0.001) {
      return 1.0;
    } else {
      return 0.0;
    }
  }

  void SetUp() override {
    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_data = std::make_shared<DataWrapper>(GetData);
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::SinglePoleIIR(
                m_data, kSinglePoleIIRTimeConstant, kFilterStep));
        m_expectedOutput = kSinglePoleIIRExpectedOutput;
        break;
      }

      case TEST_HIGH_PASS: {
        m_data = std::make_shared<DataWrapper>(GetData);
        m_filter =
            std::make_unique<LinearDigitalFilter>(LinearDigitalFilter::HighPass(
                m_data, kHighPassTimeConstant, kFilterStep));
        m_expectedOutput = kHighPassExpectedOutput;
        break;
      }

      case TEST_MOVAVG: {
        m_data = std::make_shared<DataWrapper>(GetData);
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::MovingAverage(m_data, kMovAvgTaps));
        m_expectedOutput = kMovAvgExpectedOutput;
        break;
      }

      case TEST_PULSE: {
        m_data = std::make_shared<DataWrapper>(GetPulseData);
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::MovingAverage(m_data, kMovAvgTaps));
        m_expectedOutput = 0.0;
        break;
      }
    }
  }
};

/**
 * Test if the linear digital filters produce consistent output
 */
TEST_P(FilterOutputTest, FilterOutput) {
  m_data->Reset();

  double filterOutput = 0.0;
  for (double t = 0.0; t < kFilterTime; t += kFilterStep) {
    filterOutput = m_filter->PIDGet();
  }

  RecordProperty("FilterOutput", filterOutput);

  EXPECT_FLOAT_EQ(m_expectedOutput, filterOutput)
      << "Filter output didn't match expected value";
}

INSTANTIATE_TEST_CASE_P(Test, FilterOutputTest,
                        testing::Values(TEST_SINGLE_POLE_IIR, TEST_HIGH_PASS,
                                        TEST_MOVAVG, TEST_PULSE), );
