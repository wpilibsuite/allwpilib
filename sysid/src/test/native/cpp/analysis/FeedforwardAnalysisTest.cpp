// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <bitset>
#include <cmath>
#include <span>

#include <gtest/gtest.h>
#include <units/time.h>
#include <units/voltage.h>

#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/ArmSim.h"
#include "sysid/analysis/ElevatorSim.h"
#include "sysid/analysis/FeedforwardAnalysis.h"
#include "sysid/analysis/SimpleMotorSim.h"

namespace {

enum Movements : uint32_t {
  SLOW_FORWARD,
  SLOW_BACKWARD,
  FAST_FORWARD,
  FAST_BACKWARD
};

inline constexpr int MOVEMENT_COMBINATIONS = 16;

/**
 * Return simulated test data for a given simulation model.
 *
 * @tparam Model The model type.
 * @param model The simulation model.
 * @param movements Which movements to do.
 */
template <typename Model>
sysid::Storage CollectData(Model& model, std::bitset<4> movements) {
  constexpr auto U_STEP = 0.25_V / 1_s;
  constexpr units::volt_t U_MAX = 7_V;
  constexpr units::second_t T = 5_ms;
  constexpr units::second_t TEST_DURATION = 5_s;

  sysid::Storage storage;
  auto& [slowForward, slowBackward, fastForward, fastBackward] = storage;
  auto voltage = 0_V;

  // Slow forward
  if (movements.test(Movements::SLOW_FORWARD)) {
    model.Reset();
    voltage = 0_V;
    for (int i = 0; i < (TEST_DURATION / T).value(); ++i) {
      slowForward.emplace_back(sysid::PreparedData{
          i * T, voltage.value(), model.GetPosition(), model.GetVelocity(), T,
          model.GetAcceleration(voltage), std::cos(model.GetPosition()),
          std::sin(model.GetPosition())});

      model.Update(voltage, T);
      voltage += U_STEP * T;
    }
  }

  // Slow backward
  if (movements.test(Movements::SLOW_BACKWARD)) {
    model.Reset();
    voltage = 0_V;
    for (int i = 0; i < (TEST_DURATION / T).value(); ++i) {
      slowBackward.emplace_back(sysid::PreparedData{
          i * T, voltage.value(), model.GetPosition(), model.GetVelocity(), T,
          model.GetAcceleration(voltage), std::cos(model.GetPosition()),
          std::sin(model.GetPosition())});

      model.Update(voltage, T);
      voltage -= U_STEP * T;
    }
  }

  // Fast forward
  if (movements.test(Movements::FAST_FORWARD)) {
    model.Reset();
    voltage = 0_V;
    for (int i = 0; i < (TEST_DURATION / T).value(); ++i) {
      fastForward.emplace_back(sysid::PreparedData{
          i * T, voltage.value(), model.GetPosition(), model.GetVelocity(), T,
          model.GetAcceleration(voltage), std::cos(model.GetPosition()),
          std::sin(model.GetPosition())});

      model.Update(voltage, T);
      voltage = U_MAX;
    }
  }

  // Fast backward
  if (movements.test(Movements::FAST_BACKWARD)) {
    model.Reset();
    voltage = 0_V;
    for (int i = 0; i < (TEST_DURATION / T).value(); ++i) {
      fastBackward.emplace_back(sysid::PreparedData{
          i * T, voltage.value(), model.GetPosition(), model.GetVelocity(), T,
          model.GetAcceleration(voltage), std::cos(model.GetPosition()),
          std::sin(model.GetPosition())});

      model.Update(voltage, T);
      voltage = -U_MAX;
    }
  }

  return storage;
}

/**
 * Asserts success if the gains contain NaNs or are too far from their expected
 * values.
 *
 * @param expectedGains The expected feedforward gains.
 * @param actualGains The calculated feedforward gains.
 * @param tolerances The tolerances for the coefficient comparisons.
 */
testing::AssertionResult FitIsBad(std::span<const double> expectedGains,
                                  std::span<const double> actualGains,
                                  std::span<const double> tolerances) {
  // Check for NaN
  for (const auto& coeff : actualGains) {
    if (std::isnan(coeff)) {
      return testing::AssertionSuccess();
    }
  }

  for (size_t i = 0; i < expectedGains.size(); ++i) {
    if (std::abs(expectedGains[i] - actualGains[i]) >= tolerances[i]) {
      return testing::AssertionSuccess();
    }
  }

  auto result = testing::AssertionFailure();

  result << "\n";
  for (size_t i = 0; i < expectedGains.size(); ++i) {
    if (i == 0) {
      result << "S";
    } else if (i == 1) {
      result << "V";
    } else if (i == 2) {
      result << "A";
    } else if (i == 3) {
      result << "G";
    } else if (i == 4) {
      result << "offset";
    }

    result << ":\n";
    result << "  expected " << expectedGains[i] << ",\n";
    result << "  actual " << actualGains[i] << ",\n";
    result << "  diff " << std::abs(expectedGains[i] - actualGains[i]) << "\n";
  }

  return result;
}

/**
 * Asserts that two arrays are equal.
 *
 * @param expected The expected array.
 * @param actual The actual array.
 * @param tolerances The tolerances for the element comparisons.
 */
void ExpectArrayNear(std::span<const double> expected,
                     std::span<const double> actual,
                     std::span<const double> tolerances) {
  // Check size
  const size_t size = expected.size();
  EXPECT_EQ(size, actual.size());
  EXPECT_EQ(size, tolerances.size());

  // Check elements
  for (size_t i = 0; i < size; ++i) {
    EXPECT_NEAR(expected[i], actual[i], tolerances[i]) << "where i = " << i;
  }
}

/**
 * @tparam Model The model type.
 * @param model The simulation model.
 * @param type The analysis type.
 * @param expectedGains The expected feedforward gains.
 * @param tolerances The tolerances for the coefficient comparisons.
 */
template <typename Model>
void RunTests(Model& model, const sysid::AnalysisType& type,
              std::span<const double> expectedGains,
              std::span<const double> tolerances) {
  // Iterate through all combinations of movements
  for (int movements = 0; movements < MOVEMENT_COMBINATIONS; ++movements) {
    try {
      auto ff =
          sysid::CalculateFeedforwardGains(CollectData(model, movements), type);

      ExpectArrayNear(expectedGains, ff.coeffs, tolerances);
    } catch (sysid::InsufficientSamplesError&) {
      // If calculation threw an exception, confirm at least one of the gains
      // doesn't match
      auto ff = sysid::CalculateFeedforwardGains(CollectData(model, movements),
                                                 type, false);
      EXPECT_TRUE(FitIsBad(expectedGains, ff.coeffs, tolerances));
    }
  }
}

}  // namespace

TEST(FeedforwardAnalysisTest, Arm) {
  {
    constexpr double S = 1.01;
    constexpr double V = 3.060;
    constexpr double A = 0.327;
    constexpr double G = 0.211;

    for (const auto& offset : {-2.0, -1.0, 0.0, 1.0, 2.0}) {
      sysid::ArmSim model{S, V, A, G, offset};

      RunTests(model, sysid::analysis::ARM, {{S, V, A, G, offset}},
               {{8e-3, 8e-3, 8e-3, 8e-3, 3e-2}});
    }
  }

  {
    constexpr double S = 0.547;
    constexpr double V = 0.0693;
    constexpr double A = 0.1170;
    constexpr double G = 0.122;

    for (const auto& offset : {-2.0, -1.0, 0.0, 1.0, 2.0}) {
      sysid::ArmSim model{S, V, A, G, offset};

      RunTests(model, sysid::analysis::ARM, {{S, V, A, G, offset}},
               {{8e-3, 8e-3, 8e-3, 8e-3, 5e-2}});
    }
  }
}

TEST(FeedforwardAnalysisTest, Elevator) {
  {
    constexpr double S = 1.01;
    constexpr double V = 3.060;
    constexpr double A = 0.327;
    constexpr double G = -0.211;

    sysid::ElevatorSim model{S, V, A, G};

    RunTests(model, sysid::analysis::ELEVATOR, {{S, V, A, G}},
             {{8e-3, 8e-3, 8e-3, 8e-3}});
  }

  {
    constexpr double S = 0.547;
    constexpr double V = 0.0693;
    constexpr double A = 0.1170;
    constexpr double G = -0.122;

    sysid::ElevatorSim model{S, V, A, G};

    RunTests(model, sysid::analysis::ELEVATOR, {{S, V, A, G}},
             {{8e-3, 8e-3, 8e-3, 8e-3}});
  }
}

TEST(FeedforwardAnalysisTest, Simple) {
  {
    constexpr double S = 1.01;
    constexpr double V = 3.060;
    constexpr double A = 0.327;

    sysid::SimpleMotorSim model{S, V, A};

    RunTests(model, sysid::analysis::SIMPLE, {{S, V, A}}, {{8e-3, 8e-3, 8e-3}});
  }

  {
    constexpr double S = 0.547;
    constexpr double V = 0.0693;
    constexpr double A = 0.1170;

    sysid::SimpleMotorSim model{S, V, A};

    RunTests(model, sysid::analysis::SIMPLE, {{S, V, A}}, {{8e-3, 8e-3, 8e-3}});
  }
}
