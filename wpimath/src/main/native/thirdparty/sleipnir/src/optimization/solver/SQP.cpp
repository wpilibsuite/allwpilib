// Copyright (c) Sleipnir contributors

#include "sleipnir/optimization/solver/SQP.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <limits>

#include <Eigen/SparseCholesky>
#include <wpi/SmallVector.h>

#include "optimization/RegularizedLDLT.hpp"
#include "optimization/solver/util/ErrorEstimate.hpp"
#include "optimization/solver/util/FeasibilityRestoration.hpp"
#include "optimization/solver/util/Filter.hpp"
#include "optimization/solver/util/IsLocallyInfeasible.hpp"
#include "optimization/solver/util/KKTError.hpp"
#include "sleipnir/autodiff/Gradient.hpp"
#include "sleipnir/autodiff/Hessian.hpp"
#include "sleipnir/autodiff/Jacobian.hpp"
#include "sleipnir/optimization/SolverExitCondition.hpp"
#include "sleipnir/util/Print.hpp"
#include "sleipnir/util/Spy.hpp"
#include "util/ScopeExit.hpp"
#include "util/ToMilliseconds.hpp"

// See docs/algorithms.md#Works_cited for citation definitions.

namespace sleipnir {

void SQP(std::span<Variable> decisionVariables,
         std::span<Variable> equalityConstraints, Variable& f,
         function_ref<bool(const SolverIterationInfo& info)> callback,
         const SolverConfig& config, Eigen::VectorXd& x, SolverStatus* status) {
  const auto solveStartTime = std::chrono::system_clock::now();

  // Map decision variables and constraints to VariableMatrices for Lagrangian
  VariableMatrix xAD{decisionVariables};
  xAD.SetValue(x);
  VariableMatrix c_eAD{equalityConstraints};

  // Create autodiff variables for y for Lagrangian
  VariableMatrix yAD(equalityConstraints.size());
  for (auto& y : yAD) {
    y.SetValue(0.0);
  }

  // Lagrangian L
  //
  // L(xₖ, yₖ) = f(xₖ) − yₖᵀcₑ(xₖ)
  auto L = f - (yAD.T() * c_eAD)(0);

  // Equality constraint Jacobian Aₑ
  //
  //         [∇ᵀcₑ₁(xₖ)]
  // Aₑ(x) = [∇ᵀcₑ₂(xₖ)]
  //         [    ⋮    ]
  //         [∇ᵀcₑₘ(xₖ)]
  Jacobian jacobianCe{c_eAD, xAD};
  Eigen::SparseMatrix<double> A_e = jacobianCe.Value();

  // Gradient of f ∇f
  Gradient gradientF{f, xAD};
  Eigen::SparseVector<double> g = gradientF.Value();

  // Hessian of the Lagrangian H
  //
  // Hₖ = ∇²ₓₓL(xₖ, yₖ)
  Hessian hessianL{L, xAD};
  Eigen::SparseMatrix<double> H = hessianL.Value();

  Eigen::VectorXd y = yAD.Value();
  Eigen::VectorXd c_e = c_eAD.Value();

  // Check for overconstrained problem
  if (equalityConstraints.size() > decisionVariables.size()) {
    if (config.diagnostics) {
      sleipnir::println("The problem has too few degrees of freedom.");
      sleipnir::println(
          "Violated constraints (cₑ(x) = 0) in order of declaration:");
      for (int row = 0; row < c_e.rows(); ++row) {
        if (c_e(row) < 0.0) {
          sleipnir::println("  {}/{}: {} = 0", row + 1, c_e.rows(), c_e(row));
        }
      }
    }

    status->exitCondition = SolverExitCondition::kTooFewDOFs;
    return;
  }

  // Check whether initial guess has finite f(xₖ) and cₑ(xₖ)
  if (!std::isfinite(f.Value()) || !c_e.allFinite()) {
    status->exitCondition =
        SolverExitCondition::kNonfiniteInitialCostOrConstraints;
    return;
  }

  // Sparsity pattern files written when spy flag is set in SolverConfig
  std::ofstream H_spy;
  std::ofstream A_e_spy;
  if (config.spy) {
    A_e_spy.open("A_e.spy");
    H_spy.open("H.spy");
  }

  if (config.diagnostics) {
    sleipnir::println("Error tolerance: {}\n", config.tolerance);
  }

  std::chrono::system_clock::time_point iterationsStartTime;

  int iterations = 0;

  // Prints final diagnostics when the solver exits
  scope_exit exit{[&] {
    status->cost = f.Value();

    if (config.diagnostics) {
      auto solveEndTime = std::chrono::system_clock::now();

      sleipnir::println("\nSolve time: {:.3f} ms",
                        ToMilliseconds(solveEndTime - solveStartTime));
      sleipnir::println("  ↳ {:.3f} ms (solver setup)",
                        ToMilliseconds(iterationsStartTime - solveStartTime));
      if (iterations > 0) {
        sleipnir::println(
            "  ↳ {:.3f} ms ({} solver iterations; {:.3f} ms average)",
            ToMilliseconds(solveEndTime - iterationsStartTime), iterations,
            ToMilliseconds((solveEndTime - iterationsStartTime) / iterations));
      }
      sleipnir::println("");

      sleipnir::println("{:^8}   {:^10}   {:^14}   {:^6}", "autodiff",
                        "setup (ms)", "avg solve (ms)", "solves");
      sleipnir::println("{:=^47}", "");
      constexpr auto format = "{:^8}   {:10.3f}   {:14.3f}   {:6}";
      sleipnir::println(format, "∇f(x)",
                        gradientF.GetProfiler().SetupDuration(),
                        gradientF.GetProfiler().AverageSolveDuration(),
                        gradientF.GetProfiler().SolveMeasurements());
      sleipnir::println(format, "∇²ₓₓL", hessianL.GetProfiler().SetupDuration(),
                        hessianL.GetProfiler().AverageSolveDuration(),
                        hessianL.GetProfiler().SolveMeasurements());
      sleipnir::println(format, "∂cₑ/∂x",
                        jacobianCe.GetProfiler().SetupDuration(),
                        jacobianCe.GetProfiler().AverageSolveDuration(),
                        jacobianCe.GetProfiler().SolveMeasurements());
      sleipnir::println("");
    }
  }};

  Filter filter{f};

  // Kept outside the loop so its storage can be reused
  wpi::SmallVector<Eigen::Triplet<double>> triplets;

  RegularizedLDLT solver;

  // Variables for determining when a step is acceptable
  constexpr double α_red_factor = 0.5;
  int acceptableIterCounter = 0;

  int fullStepRejectedCounter = 0;

  // Error estimate
  double E_0 = std::numeric_limits<double>::infinity();

  if (config.diagnostics) {
    iterationsStartTime = std::chrono::system_clock::now();
  }

  while (E_0 > config.tolerance &&
         acceptableIterCounter < config.maxAcceptableIterations) {
    std::chrono::system_clock::time_point innerIterStartTime;
    if (config.diagnostics) {
      innerIterStartTime = std::chrono::system_clock::now();
    }

    // Check for local equality constraint infeasibility
    if (IsEqualityLocallyInfeasible(A_e, c_e)) {
      if (config.diagnostics) {
        sleipnir::println(
            "The problem is locally infeasible due to violated equality "
            "constraints.");
        sleipnir::println(
            "Violated constraints (cₑ(x) = 0) in order of declaration:");
        for (int row = 0; row < c_e.rows(); ++row) {
          if (c_e(row) < 0.0) {
            sleipnir::println("  {}/{}: {} = 0", row + 1, c_e.rows(), c_e(row));
          }
        }
      }

      status->exitCondition = SolverExitCondition::kLocallyInfeasible;
      return;
    }

    // Check for diverging iterates
    if (x.lpNorm<Eigen::Infinity>() > 1e20 || !x.allFinite()) {
      status->exitCondition = SolverExitCondition::kDivergingIterates;
      return;
    }

    // Write out spy file contents if that's enabled
    if (config.spy) {
      // Gap between sparsity patterns
      if (iterations > 0) {
        A_e_spy << "\n";
        H_spy << "\n";
      }

      Spy(H_spy, H);
      Spy(A_e_spy, A_e);
    }

    // Call user callback
    if (callback({iterations, x, Eigen::VectorXd::Zero(0), g, H, A_e,
                  Eigen::SparseMatrix<double>{}})) {
      status->exitCondition = SolverExitCondition::kCallbackRequestedStop;
      return;
    }

    // lhs = [H   Aₑᵀ]
    //       [Aₑ   0 ]
    //
    // Don't assign upper triangle because solver only uses lower triangle.
    const Eigen::SparseMatrix<double> topLeft =
        H.triangularView<Eigen::Lower>();
    triplets.clear();
    triplets.reserve(topLeft.nonZeros() + A_e.nonZeros());
    for (int col = 0; col < H.cols(); ++col) {
      // Append column of H + AᵢᵀΣAᵢ lower triangle in top-left quadrant
      for (Eigen::SparseMatrix<double>::InnerIterator it{topLeft, col}; it;
           ++it) {
        triplets.emplace_back(it.row(), it.col(), it.value());
      }
      // Append column of Aₑ in bottom-left quadrant
      for (Eigen::SparseMatrix<double>::InnerIterator it{A_e, col}; it; ++it) {
        triplets.emplace_back(H.rows() + it.row(), it.col(), it.value());
      }
    }
    Eigen::SparseMatrix<double> lhs(
        decisionVariables.size() + equalityConstraints.size(),
        decisionVariables.size() + equalityConstraints.size());
    lhs.setFromSortedTriplets(triplets.begin(), triplets.end(),
                              [](const auto&, const auto& b) { return b; });

    // rhs = −[∇f − Aₑᵀy]
    //        [   cₑ    ]
    Eigen::VectorXd rhs{x.rows() + y.rows()};
    rhs.segment(0, x.rows()) = -(g - A_e.transpose() * y);
    rhs.segment(x.rows(), y.rows()) = -c_e;

    // Solve the Newton-KKT system
    //
    // [H   Aₑᵀ][ pₖˣ] = −[∇f − Aₑᵀy]
    // [Aₑ   0 ][−pₖʸ]    [   cₑ    ]
    solver.Compute(lhs, equalityConstraints.size(), config.tolerance / 10.0);
    Eigen::VectorXd step{x.rows() + y.rows()};
    if (solver.Info() == Eigen::Success) {
      step = solver.Solve(rhs);
    } else {
      // The regularization procedure failed due to a rank-deficient equality
      // constraint Jacobian with linearly dependent constraints
      status->exitCondition = SolverExitCondition::kLocallyInfeasible;
      return;
    }

    // step = [ pₖˣ]
    //        [−pₖʸ]
    Eigen::VectorXd p_x = step.segment(0, x.rows());
    Eigen::VectorXd p_y = -step.segment(x.rows(), y.rows());

    constexpr double α_max = 1.0;
    double α = α_max;

    // Loop until a step is accepted. If a step becomes acceptable, the loop
    // will exit early.
    while (1) {
      Eigen::VectorXd trial_x = x + α * p_x;
      Eigen::VectorXd trial_y = y + α * p_y;

      xAD.SetValue(trial_x);

      Eigen::VectorXd trial_c_e = c_eAD.Value();

      // If f(xₖ + αpₖˣ) or cₑ(xₖ + αpₖˣ) aren't finite, reduce step size
      // immediately
      if (!std::isfinite(f.Value()) || !trial_c_e.allFinite()) {
        // Reduce step size
        α *= α_red_factor;
        continue;
      }

      // Check whether filter accepts trial iterate
      auto entry = filter.MakeEntry(trial_c_e);
      if (filter.TryAdd(entry)) {
        // Accept step
        break;
      }

      double prevConstraintViolation = c_e.lpNorm<1>();
      double nextConstraintViolation = trial_c_e.lpNorm<1>();

      // Second-order corrections
      //
      // If first trial point was rejected and constraint violation stayed the
      // same or went up, apply second-order corrections
      if (nextConstraintViolation >= prevConstraintViolation) {
        // Apply second-order corrections. See section 2.4 of [2].
        Eigen::VectorXd p_x_cor = p_x;
        Eigen::VectorXd p_y_soc = p_y;

        double α_soc = α;
        Eigen::VectorXd c_e_soc = c_e;

        bool stepAcceptable = false;
        for (int soc_iteration = 0; soc_iteration < 5 && !stepAcceptable;
             ++soc_iteration) {
          // Rebuild Newton-KKT rhs with updated constraint values.
          //
          // rhs = −[∇f − Aₑᵀy]
          //        [  cₑˢᵒᶜ  ]
          //
          // where cₑˢᵒᶜ = αc(xₖ) + c(xₖ + αpₖˣ)
          c_e_soc = α_soc * c_e_soc + trial_c_e;
          rhs.bottomRows(y.rows()) = -c_e_soc;

          // Solve the Newton-KKT system
          step = solver.Solve(rhs);

          p_x_cor = step.segment(0, x.rows());
          p_y_soc = -step.segment(x.rows(), y.rows());

          trial_x = x + α_soc * p_x_cor;
          trial_y = y + α_soc * p_y_soc;

          xAD.SetValue(trial_x);

          trial_c_e = c_eAD.Value();

          // Check whether filter accepts trial iterate
          entry = filter.MakeEntry(trial_c_e);
          if (filter.TryAdd(entry)) {
            p_x = p_x_cor;
            p_y = p_y_soc;
            α = α_soc;
            stepAcceptable = true;
          }
        }

        if (stepAcceptable) {
          // Accept step
          break;
        }
      }

      // If we got here and α is the full step, the full step was rejected.
      // Increment the full-step rejected counter to keep track of how many full
      // steps have been rejected in a row.
      if (α == α_max) {
        ++fullStepRejectedCounter;
      }

      // If the full step was rejected enough times in a row, reset the filter
      // because it may be impeding progress.
      //
      // See section 3.2 case I of [2].
      if (fullStepRejectedCounter >= 4 &&
          filter.maxConstraintViolation > entry.constraintViolation / 10.0) {
        filter.maxConstraintViolation *= 0.1;
        filter.Reset();
        continue;
      }

      // Reduce step size
      α *= α_red_factor;

      // Safety factor for the minimal step size
      constexpr double α_min_frac = 0.05;

      // If step size hit a minimum, check if the KKT error was reduced. If it
      // wasn't, report infeasible.
      if (α < α_min_frac * Filter::γConstraint) {
        double currentKKTError = KKTError(g, A_e, c_e, y);

        Eigen::VectorXd trial_x = x + α_max * p_x;
        Eigen::VectorXd trial_y = y + α_max * p_y;

        // Upate autodiff
        xAD.SetValue(trial_x);
        yAD.SetValue(trial_y);

        Eigen::VectorXd trial_c_e = c_eAD.Value();

        double nextKKTError =
            KKTError(gradientF.Value(), jacobianCe.Value(), trial_c_e, trial_y);

        // If the step using αᵐᵃˣ reduced the KKT error, accept it anyway
        if (nextKKTError <= 0.999 * currentKKTError) {
          α = α_max;

          // Accept step
          break;
        }

        auto initialEntry = filter.MakeEntry(c_e);

        // Feasibility restoration phase
        Eigen::VectorXd fr_x = x;
        SolverStatus fr_status;
        FeasibilityRestoration(
            decisionVariables, equalityConstraints,
            [&](const SolverIterationInfo& info) {
              trial_x = info.x.segment(0, decisionVariables.size());
              xAD.SetValue(trial_x);

              trial_c_e = c_eAD.Value();

              // If current iterate is acceptable to normal filter and
              // constraint violation has sufficiently reduced, stop
              // feasibility restoration
              entry = filter.MakeEntry(trial_c_e);
              if (filter.IsAcceptable(entry) &&
                  entry.constraintViolation <
                      0.9 * initialEntry.constraintViolation) {
                return true;
              }

              return false;
            },
            config, fr_x, &fr_status);

        if (fr_status.exitCondition ==
            SolverExitCondition::kCallbackRequestedStop) {
          p_x = fr_x - x;

          // Lagrange mutliplier estimates
          //
          //   y = (AₑAₑᵀ)⁻¹Aₑ∇f
          //
          // See equation (19.37) of [1].
          {
            xAD.SetValue(fr_x);

            A_e = jacobianCe.Value();
            g = gradientF.Value();

            // lhs = AₑAₑᵀ
            Eigen::SparseMatrix<double> lhs = A_e * A_e.transpose();

            // rhs = Aₑ∇f
            Eigen::VectorXd rhs = A_e * g;

            Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> yEstimator{lhs};
            Eigen::VectorXd sol = yEstimator.solve(rhs);

            p_y = y - sol.block(0, 0, y.rows(), 1);
          }

          α = 1.0;

          // Accept step
          break;
        } else if (fr_status.exitCondition == SolverExitCondition::kSuccess) {
          status->exitCondition = SolverExitCondition::kLocallyInfeasible;
          x = fr_x;
          return;
        } else {
          status->exitCondition =
              SolverExitCondition::kFeasibilityRestorationFailed;
          x = fr_x;
          return;
        }
      }
    }

    // If full step was accepted, reset full-step rejected counter
    if (α == α_max) {
      fullStepRejectedCounter = 0;
    }

    // Handle very small search directions by letting αₖ = αₖᵐᵃˣ when
    // max(|pₖˣ(i)|/(1 + |xₖ(i)|)) < 10ε_mach.
    //
    // See section 3.9 of [2].
    double maxStepScaled = 0.0;
    for (int row = 0; row < x.rows(); ++row) {
      maxStepScaled = std::max(maxStepScaled,
                               std::abs(p_x(row)) / (1.0 + std::abs(x(row))));
    }
    if (maxStepScaled < 10.0 * std::numeric_limits<double>::epsilon()) {
      α = α_max;
    }

    // xₖ₊₁ = xₖ + αₖpₖˣ
    // yₖ₊₁ = yₖ + αₖpₖʸ
    x += α * p_x;
    y += α * p_y;

    // Update autodiff for Jacobians and Hessian
    xAD.SetValue(x);
    yAD.SetValue(y);
    A_e = jacobianCe.Value();
    g = gradientF.Value();
    H = hessianL.Value();

    c_e = c_eAD.Value();

    // Update the error estimate
    E_0 = ErrorEstimate(g, A_e, c_e, y);
    if (E_0 < config.acceptableTolerance) {
      ++acceptableIterCounter;
    } else {
      acceptableIterCounter = 0;
    }

    const auto innerIterEndTime = std::chrono::system_clock::now();

    // Diagnostics for current iteration
    if (config.diagnostics) {
      if (iterations % 20 == 0) {
        sleipnir::println("{:^4}   {:^9}  {:^13}  {:^13}  {:^13}", "iter",
                          "time (ms)", "error", "cost", "infeasibility");
        sleipnir::println("{:=^61}", "");
      }

      sleipnir::println("{:4}   {:9.3f}  {:13e}  {:13e}  {:13e}", iterations,
                        ToMilliseconds(innerIterEndTime - innerIterStartTime),
                        E_0, f.Value(), c_e.lpNorm<1>());
    }

    ++iterations;

    // Check for max iterations
    if (iterations >= config.maxIterations) {
      status->exitCondition = SolverExitCondition::kMaxIterationsExceeded;
      return;
    }

    // Check for max wall clock time
    if (innerIterEndTime - solveStartTime > config.timeout) {
      status->exitCondition = SolverExitCondition::kTimeout;
      return;
    }

    // Check for solve to acceptable tolerance
    if (E_0 > config.tolerance &&
        acceptableIterCounter == config.maxAcceptableIterations) {
      status->exitCondition = SolverExitCondition::kSolvedToAcceptableTolerance;
      return;
    }
  }
}

}  // namespace sleipnir
