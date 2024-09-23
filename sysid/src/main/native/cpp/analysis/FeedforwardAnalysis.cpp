// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/FeedforwardAnalysis.h"

#include <array>
#include <bitset>
#include <cmath>
#include <string>
#include <vector>

#include <Eigen/Eigenvalues>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <units/math.h>
#include <units/time.h>

#include "sysid/analysis/OLS.h"

namespace sysid {

/**
 * Populates OLS data for the following models:
 *
 * Simple, Drivetrain, DrivetrainAngular:
 *
 *   (xₖ₊₁ − xₖ)/τ = αxₖ + βuₖ + γ sgn(xₖ)
 *
 * Elevator:
 *
 *   (xₖ₊₁ − xₖ)/τ = αxₖ + βuₖ + γ sgn(xₖ) + δ
 *
 * Arm:
 *
 *   (xₖ₊₁ − xₖ)/τ = αxₖ + βuₖ + γ sgn(xₖ) + δ cos(angle) + ε sin(angle)
 *
 * OLS performs best with the noisiest variable as the dependent variable, so we
 * regress acceleration in terms of the other variables.
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
  // Fill in X and y row-wise
  for (size_t sample = 0; sample < d.size(); ++sample) {
    const auto& pt = d[sample];

    // Set the velocity term (for α)
    X(sample, 0) = pt.velocity;

    // Set the voltage term (for β)
    X(sample, 1) = pt.voltage;

    // Set the intercept term (for γ)
    X(sample, 2) = std::copysign(1, pt.velocity);

    // Set test-specific variables
    if (type == analysis::kElevator) {
      // Set the gravity term (for δ)
      X(sample, 3) = 1.0;
    } else if (type == analysis::kArm) {
      // Set the cosine and sine terms (for δ and ε)
      X(sample, 3) = pt.cos;
      X(sample, 4) = pt.sin;
    }

    // Set the dependent variable (acceleration)
    y(sample) = pt.acceleration;
  }
}

/**
 * Throws an InsufficientSamplesError if the collected data is poor for OLS.
 *
 * @param X The collected data in matrix form for OLS.
 * @param type The analysis type.
 */
static void CheckOLSDataQuality(const Eigen::MatrixXd& X,
                                const AnalysisType& type) {
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigSolver{X.transpose() * X};
  const Eigen::VectorXd& eigvals = eigSolver.eigenvalues();
  const Eigen::MatrixXd& eigvecs = eigSolver.eigenvectors();

  // Bits are Ks, Kv, Ka, Kg, offset
  std::bitset<5> badGains;

  constexpr double threshold = 10.0;

  // For n x n matrix XᵀX, need n nonzero eigenvalues for good fit
  for (int row = 0; row < eigvals.rows(); ++row) {
    // Find row of eigenvector with largest magnitude. This determines the
    // primary regression variable that corresponds to the eigenvalue.
    int maxIndex;
    double maxCoeff = eigvecs.col(row).cwiseAbs().maxCoeff(&maxIndex);

    // Check whether the eigenvector component along the regression variable's
    // direction is below the threshold. If it is, the regression variable's fit
    // is bad.
    if (std::abs(eigvals(row) * maxCoeff) <= threshold) {
      // Fit for α is bad
      if (maxIndex == 0) {
        // Affects Kv
        badGains.set(1);
      }

      // Fit for β is bad
      if (maxIndex == 1) {
        // Affects all gains
        badGains.set();
        break;
      }

      // Fit for γ is bad
      if (maxIndex == 2) {
        // Affects Ks
        badGains.set(0);
      }

      // Fit for δ is bad
      if (maxIndex == 3) {
        if (type == analysis::kElevator) {
          // Affects Kg
          badGains.set(3);
        } else if (type == analysis::kArm) {
          // Affects Kg and offset
          badGains.set(3);
          badGains.set(4);
        }
      }

      // Fit for ε is bad
      if (maxIndex == 4) {
        // Affects Kg and offset
        badGains.set(3);
        badGains.set(4);
      }
    }
  }

  // If any gains are bad, throw an error
  if (badGains.any()) {
    // Create list of bad gain names
    constexpr std::array gainNames{"Ks", "Kv", "Ka", "Kg", "offset"};
    std::vector<std::string_view> badGainsList;
    for (size_t i = 0; i < badGains.size(); ++i) {
      if (badGains.test(i)) {
        badGainsList.emplace_back(gainNames[i]);
      }
    }

    std::string error = fmt::format("Insufficient samples to compute {}.\n\n",
                                    fmt::join(badGainsList, ", "));

    // If all gains are bad, the robot may not have moved
    if (badGains.all()) {
      error += "Either no data was collected or the robot didn't move.\n\n";
    }

    // Append guidance for fixing the data
    error +=
        "Ensure the data has:\n\n"
        "  * at least 2 steady-state velocity events to separate Ks from Kv\n"
        "  * at least 1 acceleration event to find Ka\n"
        "  * for elevators, enough vertical motion to measure gravity\n"
        "  * for arms, enough range of motion to measure gravity and encoder "
        "offset\n";
    throw InsufficientSamplesError{error};
  }
}

OLSResult CalculateFeedforwardGains(const Storage& data,
                                    const AnalysisType& type,
                                    bool throwOnBadData) {
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

  // Check quality of collected data
  if (throwOnBadData) {
    CheckOLSDataQuality(X, type);
  }

  std::vector<double> gains;
  gains.reserve(X.rows());

  auto ols = OLS(X, y);

  // Calculate feedforward gains
  //
  // See docs/ols-derivations.md for more details.
  {
    // dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x)
    // dx/dt = αx + βu + γ sgn(x)

    // α = -Kv/Ka
    // β = 1/Ka
    // γ = -Ks/Ka
    double α = ols.coeffs[0];
    double β = ols.coeffs[1];
    double γ = ols.coeffs[2];

    // Ks = -γ/β
    // Kv = -α/β
    // Ka = 1/β
    gains.emplace_back(-γ / β);
    gains.emplace_back(-α / β);
    gains.emplace_back(1 / β);

    if (type == analysis::kElevator) {
      // dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x) - Kg/Ka
      // dx/dt = αx + βu + γ sgn(x) + δ

      // δ = -Kg/Ka
      double δ = ols.coeffs[3];

      // Kg = -δ/β
      gains.emplace_back(-δ / β);
    }

    if (type == analysis::kArm) {
      // dx/dt = -Kv/Ka x + 1/Ka u - Ks/Ka sgn(x)
      //           - Kg/Ka cos(offset) cos(angle)                   NOLINT
      //           + Kg/Ka sin(offset) sin(angle)                   NOLINT
      // dx/dt = αx + βu + γ sgn(x) + δ cos(angle) + ε sin(angle)   NOLINT

      // δ = -Kg/Ka cos(offset)
      // ε = Kg/Ka sin(offset)
      double δ = ols.coeffs[3];
      double ε = ols.coeffs[4];

      // Kg = hypot(δ, ε)/β      NOLINT
      // offset = atan2(ε, -δ)   NOLINT
      gains.emplace_back(std::hypot(δ, ε) / β);
      gains.emplace_back(std::atan2(ε, -δ));
    }
  }

  // Gains are Ks, Kv, Ka, Kg (elevator/arm only), offset (arm only)
  return OLSResult{gains, ols.rSquared, ols.rmse};
}

}  // namespace sysid
