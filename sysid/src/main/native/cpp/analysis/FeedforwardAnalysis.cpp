// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/FeedforwardAnalysis.h"

#include <cmath>

#include <units/math.h>
#include <units/time.h>

#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/FilteringUtils.h"
#include "sysid/analysis/OLS.h"

using namespace sysid;

/**
 * Populates OLS data for (xₖ₊₁ − xₖ)/τ = αxₖ + βuₖ + γ sgn(xₖ).
 *
 * @param d List of characterization data.
 * @param type Type of system being identified.
 * @param X Vector representation of X in y = Xβ.
 * @param y Vector representation of y in y = Xβ.
 */
static void PopulateOLSData(const std::vector<PreparedData>& d,
                            const AnalysisType& type,
                            Eigen::Block<Eigen::MatrixXd> X,
                            Eigen::VectorBlock<Eigen::VectorXd> y) {
  for (size_t sample = 0; sample < d.size(); ++sample) {
    const auto& pt = d[sample];

    // Add the velocity term (for α)
    X(sample, 0) = pt.velocity;

    // Add the voltage term (for β)
    X(sample, 1) = pt.voltage;

    // Add the intercept term (for γ)
    X(sample, 2) = std::copysign(1, pt.velocity);

    // Add test-specific variables
    if (type == analysis::kElevator) {
      // Add the gravity term (for Kg)
      X(sample, 3) = 1.0;
    } else if (type == analysis::kArm) {
      // Add the cosine and sine terms (for Kg)
      X(sample, 3) = pt.cos;
      X(sample, 4) = pt.sin;
    }

    // Add the dependent variable (acceleration)
    y(sample) = pt.acceleration;
  }
}

std::tuple<std::vector<double>, double, double>
sysid::CalculateFeedforwardGains(const Storage& data,
                                 const AnalysisType& type) {
  // Iterate through the data and add it to our raw vector.
  const auto& [slowForward, slowBackward, fastForward, fastBackward] = data;

  const auto size = slowForward.size() + slowBackward.size() +
                    fastForward.size() + fastBackward.size();

  // Create a raw vector of doubles with our data in it.
  Eigen::MatrixXd X{size, type.independentVariables};
  Eigen::VectorXd y{size};

  int rowOffset = 0;
  PopulateOLSData(slowForward, type,
                  X.block(rowOffset, 0, slowForward.size(), X.cols()),
                  y.segment(rowOffset, slowForward.size()));

  rowOffset += slowForward.size();
  PopulateOLSData(slowBackward, type,
                  X.block(rowOffset, 0, slowBackward.size(), X.cols()),
                  y.segment(rowOffset, slowBackward.size()));

  rowOffset += slowBackward.size();
  PopulateOLSData(fastForward, type,
                  X.block(rowOffset, 0, fastForward.size(), X.cols()),
                  y.segment(rowOffset, fastForward.size()));

  rowOffset += fastForward.size();
  PopulateOLSData(fastBackward, type,
                  X.block(rowOffset, 0, fastBackward.size(), X.cols()),
                  y.segment(rowOffset, fastBackward.size()));

  // Perform OLS with accel = alpha*vel + beta*voltage + gamma*signum(vel)
  // OLS performs best with the noisiest variable as the dependent var,
  // so we regress accel in terms of the other variables.
  auto ols = sysid::OLS(X, y);
  double alpha = std::get<0>(ols)[0];  // -Kv/Ka
  double beta = std::get<0>(ols)[1];   // 1/Ka
  double gamma = std::get<0>(ols)[2];  // -Ks/Ka

  // Initialize gains list with Ks, Kv, and Ka
  std::vector<double> gains{-gamma / beta, -alpha / beta, 1 / beta};

  if (type == analysis::kElevator) {
    // Add Kg to gains list
    double delta = std::get<0>(ols)[3];  // -Kg/Ka
    gains.emplace_back(-delta / beta);
  }

  if (type == analysis::kArm) {
    double delta = std::get<0>(ols)[3];    // -Kg/Ka cos(offset)
    double epsilon = std::get<0>(ols)[4];  // Kg/Ka sin(offset)

    // Add Kg to gains list
    gains.emplace_back(std::hypot(delta, epsilon) / beta);

    // Add offset to gains list
    gains.emplace_back(std::atan2(epsilon, -delta));
  }

  // Gains are Ks, Kv, Ka, Kg (elevator/arm only), offset (arm only)
  return std::tuple{gains, std::get<1>(ols), std::get<2>(ols)};
}
