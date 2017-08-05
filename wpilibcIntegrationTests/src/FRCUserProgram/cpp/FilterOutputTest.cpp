/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Filters/LinearDigitalFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <functional>
#include <memory>
#include <random>
#include <thread>

#include "Base.h"
#include "TestBench.h"
#include "gtest/gtest.h"

using namespace frc;

enum FilterOutputTestType { TEST_SINGLE_POLE_IIR, TEST_HIGH_PASS, TEST_MOVAVG };

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
  }

  return os;
}

class DataWrapper : public PIDSource {
 public:
  explicit DataWrapper(double (*dataFunc)(double)) { m_dataFunc = dataFunc; }

  virtual void SetPIDSourceType(PIDSourceType pidSource) {}

  virtual double PIDGet() {
    m_count += TestBench::kFilterStep;
    return m_dataFunc(m_count);
  }

  void Reset() { m_count = -TestBench::kFilterStep; }

 private:
  std::function<double(double)> m_dataFunc;

  // Make sure first call to PIDGet() uses m_count == 0
  double m_count = -TestBench::kFilterStep;
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
    return 100.0 * std::sin(2.0 * M_PI * t) + 20.0 * std::cos(50.0 * M_PI * t);
  }

  void SetUp() override {
    m_data = std::make_shared<DataWrapper>(GetData);

    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::SinglePoleIIR(
                m_data, TestBench::kSinglePoleIIRTimeConstant,
                TestBench::kFilterStep));
        m_expectedOutput = TestBench::kSinglePoleIIRExpectedOutput;
        break;
      }

      case TEST_HIGH_PASS: {
        m_filter =
            std::make_unique<LinearDigitalFilter>(LinearDigitalFilter::HighPass(
                m_data, TestBench::kHighPassTimeConstant,
                TestBench::kFilterStep));
        m_expectedOutput = TestBench::kHighPassExpectedOutput;
        break;
      }

      case TEST_MOVAVG: {
        m_filter = std::make_unique<LinearDigitalFilter>(
            LinearDigitalFilter::MovingAverage(m_data, TestBench::kMovAvgTaps));
        m_expectedOutput = TestBench::kMovAvgExpectedOutput;
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
  for (double t = 0.0; t < TestBench::kFilterTime;
       t += TestBench::kFilterStep) {
    filterOutput = m_filter->PIDGet();
  }

  RecordProperty("FilterOutput", filterOutput);

  EXPECT_FLOAT_EQ(m_expectedOutput, filterOutput)
      << "Filter output didn't match expected value";
}

INSTANTIATE_TEST_CASE_P(Test, FilterOutputTest,
                        testing::Values(TEST_SINGLE_POLE_IIR, TEST_HIGH_PASS,
                                        TEST_MOVAVG));
