// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <functional>
#include <random>
#include <utility>
#include <vector>

#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Pose2d.h"
#include "frc/optimization/SimulatedAnnealing.h"

namespace frc {

/**
 * Given a list of poses, this class finds the shortest possible route that
 * visits each pose exactly once and returns to the origin pose.
 *
 * @see <a
 * href="https://en.wikipedia.org/wiki/Travelling_salesman_problem">https://en.wikipedia.org/wiki/Travelling_salesman_problem</a>
 */
class TravelingSalesman {
 public:
  /**
   * Constructs a traveling salesman problem solver with a cost function defined
   * as the 2D distance between poses.
   */
  constexpr TravelingSalesman() = default;

  /**
   * Constructs a traveling salesman problem solver with a user-provided cost
   * function.
   *
   * @param cost Function that returns the cost between two poses. The sum of
   *     the costs for every pair of poses is minimized.
   */
  explicit TravelingSalesman(std::function<double(Pose2d, Pose2d)> cost)
      : m_cost{std::move(cost)} {}

  /**
   * Finds the path through every pose that minimizes the cost. The first pose
   * in the returned array is the first pose that was passed in.
   *
   * This overload supports a statically-sized list of poses.
   *
   * @tparam Poses The length of the path and the number of poses.
   * @param poses An array of Pose2ds the path must pass through.
   * @param iterations The number of times the solver attempts to find a better
   *     random neighbor.
   * @return The optimized path as an array of Pose2ds.
   */
  template <size_t Poses>
  wpi::array<Pose2d, Poses> Solve(const wpi::array<Pose2d, Poses>& poses,
                                  int iterations) {
    SimulatedAnnealing<Vectord<Poses>> solver{
        1, &Neighbor<Poses>, [&](const Vectord<Poses>& state) {
          // Total cost is sum of all costs between adjacent pairs in path
          double sum = 0.0;
          for (int i = 0; i < state.rows(); ++i) {
            sum +=
                m_cost(poses[static_cast<int>(state(i))],
                       poses[static_cast<int>(state((i + 1) % poses.size()))]);
          }
          return sum;
        }};

    Eigen::Vector<double, Poses> initial;
    for (int i = 0; i < initial.rows(); ++i) {
      initial(i) = i;
    }

    auto indices = solver.Solve(initial, iterations);

    wpi::array<Pose2d, Poses> solution{wpi::empty_array};
    for (size_t i = 0; i < poses.size(); ++i) {
      solution[i] = poses[static_cast<int>(indices[i])];
    }

    // Rotate solution list until solution[0] = poses[0]
    std::rotate(solution.begin(),
                std::find(solution.begin(), solution.end(), poses[0]),
                solution.end());

    return solution;
  }

  /**
   * Finds the path through every pose that minimizes the cost. The first pose
   * in the returned array is the first pose that was passed in.
   *
   * This overload supports a dynamically-sized list of poses for Python to use.
   *
   * @param poses An array of Pose2ds the path must pass through.
   * @param iterations The number of times the solver attempts to find a better
   *     random neighbor.
   * @return The optimized path as an array of Pose2ds.
   */
  std::vector<Pose2d> Solve(std::span<const Pose2d> poses, int iterations) {
    SimulatedAnnealing<Eigen::VectorXd> solver{
        1.0, &Neighbor<Eigen::Dynamic>, [&](const Eigen::VectorXd& state) {
          // Total cost is sum of all costs between adjacent pairs in path
          double sum = 0.0;
          for (int i = 0; i < state.rows(); ++i) {
            sum +=
                m_cost(poses[static_cast<int>(state(i))],
                       poses[static_cast<int>(state((i + 1) % poses.size()))]);
          }
          return sum;
        }};

    Eigen::VectorXd initial{poses.size()};
    for (int i = 0; i < initial.rows(); ++i) {
      initial(i) = i;
    }

    auto indices = solver.Solve(initial, iterations);

    std::vector<Pose2d> solution;
    for (size_t i = 0; i < poses.size(); ++i) {
      solution.emplace_back(poses[static_cast<int>(indices[i])]);
    }

    // Rotate solution list until solution[0] = poses[0]
    std::rotate(solution.begin(),
                std::find(solution.begin(), solution.end(), poses[0]),
                solution.end());

    return solution;
  }

 private:
  // Default cost is distance between poses
  std::function<double(const Pose2d&, const Pose2d&)> m_cost =
      [](const Pose2d& a, const Pose2d& b) -> double {
    return units::math::hypot(a.X() - b.X(), a.Y() - b.Y()).value();
  };

  /**
   * A random neighbor is generated to try to replace the current one.
   *
   * @tparam Poses The length of the path and the number of poses.
   * @param state A vector that is a list of indices that defines the path
   *     through the path array.
   * @return Generates a random neighbor of the current state by flipping a
   *     random range in the path array.
   */
  template <int Poses>
  static Eigen::Vector<double, Poses> Neighbor(
      const Eigen::Vector<double, Poses>& state) {
    Eigen::Vector<double, Poses> proposedState = state;

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> distr{0,
                                          static_cast<int>(state.rows()) - 1};

    int rangeStart = distr(gen);
    int rangeEnd = distr(gen);
    if (rangeEnd < rangeStart) {
      std::swap(rangeStart, rangeEnd);
    }

    for (int i = rangeStart; i <= (rangeStart + rangeEnd) / 2; ++i) {
      double temp = proposedState(i, 0);
      proposedState(i, 0) = state(rangeEnd - (i - rangeStart), 0);
      proposedState(rangeEnd - (i - rangeStart), 0) = temp;
    }

    return proposedState;
  }
};

}  // namespace frc
