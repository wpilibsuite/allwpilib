// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpical_gtsam.h"

#include <gtsam/geometry/Point2.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/nonlinear/ExpressionFactorGraph.h>
#include <gtsam/nonlinear/Marginals.h>
#include <gtsam/slam/expressions.h>

#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "pose_converters.h"

// #define OPENCV_DISABLE_EIGEN_TENSOR_SUPPORT
#include <opencv2/core/eigen.hpp>

using namespace wpical;
using namespace gtsam;
using symbol_shorthand::L;
using symbol_shorthand::X;

using CameraMatrix = Eigen::Matrix<double, 3, 3>;
using DistortionMatrix = Eigen::Matrix<double, 8, 1>;

/**
 * Check if a given tag {id} was seen in a
 */
std::set<int> TagsUsed(KeyframeMap tags) {
  std::vector<int> v;
  for (const auto& [key, tags] : tags) {
    for (const TagDetection& tag : tags) {
      v.push_back(tag.id);
    }
  }
  return {v.begin(), v.end()};
}

CalResult wpical::OptimizeLayout(
    const GtsamApriltagMap& tagLayoutGuess, KeyframeMap keyframes,
    gtsam::Cal3_S2 cal,
    const std::map<int32_t, std::pair<gtsam::Pose3, gtsam::SharedNoiseModel>>&
        fixedTags,
    const gtsam::SharedNoiseModel cameraNoise) {
  ExpressionFactorGraph graph;

  // Make sure our tags are all in the map
  for (auto& [stateKey, tags] : keyframes) {
    tags.erase(std::remove_if(tags.begin(), tags.end(),
                              [&](const auto& tag) {
                                bool ret = !tagLayoutGuess.WorldToTag(tag.id);
                                std::cout << "Checking tag " << tag.id
                                          << " -- got " << ret << std::endl;
                                return ret;
                              }),
               tags.end());
  }
  std::erase_if(keyframes, [](auto& kv) { return kv.second.empty(); });

  // constrain fixed(ish) tags - future work can investigate partial pose priors
  for (const auto& [tagId, info] : fixedTags) {
    graph.addPrior(L(tagId), std::get<0>(info), std::get<1>(info));
  }

  // Add all our tag observation factors
  for (const auto& [stateKey, tags] : keyframes) {
    for (const TagDetection& tag : tags) {
      std::cout << "Adding factors for tag " << tag.id << std::endl;
      auto worldPcorners =
          tagLayoutGuess.WorldToCornersFactor(Pose3_{L(tag.id)});

      // add each tag corner
      constexpr int NUM_CORNERS = 4;
      for (size_t i = 0; i < NUM_CORNERS; i++) {
        // Decision variable - where our camera is in the world
        const Pose3_ worldTcamera_fac(stateKey);
        // Where we'd predict the i'th corner of the tag to be
        const auto prediction = PredictLandmarkImageLocationFactor(
            worldTcamera_fac, cal, worldPcorners[i]);
        // where we saw the i'th corner in the image
        Point2 measurement = {tag.corners[i].x, tag.corners[i].y};
        // Add this prediction/measurement pair to our graph
        graph.addExpressionFactor(prediction, measurement, cameraNoise);
      }
    }
  }

  // Initial guess for our optimizer. Needs to be in the right ballpark, but
  // accuracy doesn't super matter
  Values initial;

  // Guess for all camera poses based on tag layout JSON
  for (const auto& [stateKey, tags] : keyframes) {
    if (!tags.size()) {
      std::cerr << "Can't guess pose of camera for observation " << stateKey
                << " (no tags in observation)" << std::endl;
      continue;
    }

    auto worldTcam_guess =
        EstimateWorldTCam_SingleTag(*tags.begin(), tagLayoutGuess, cal.K());
    if (!worldTcam_guess) {
      std::cerr << "Can't guess pose of camera for observation " << stateKey
                << std::endl;
    } else {
      initial.insert<Pose3>(stateKey, *worldTcam_guess);
    }
  }

  // Guess for tag locations = tag layout json
  for (int id : TagsUsed(keyframes)) {
    if (auto worldTtag = tagLayoutGuess.WorldToTag(id)) {
      initial.insert(L(id), worldTtag.value());
    } else {
      std::cerr << "Can't guess pose of tag " << id << std::endl;
    }
  }

  /* Optimize the graph and print results */
  std::cout << "==========================\ninitial error = "
            << graph.error(initial) << std::endl;
  auto start = std::chrono::steady_clock::now();

  DoglegParams params;
  params.verbosity = NonlinearOptimizerParams::ERROR;
  // params.relativeErrorTol = 1e-3;
  // params.absoluteErrorTol = 1e-3;

  // Create initial optimizer
  DoglegOptimizer optimizer{graph, initial, params};

  // Run full optimization until convergence.
  Values result;
  try {
    result = optimizer.optimize();
  } catch (std::exception* e) {
    std::cerr << e->what();
    return {};
  }

  auto end = std::chrono::steady_clock::now();
  auto dt = end - start;
  uint64_t microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(dt).count();

  std::cout << "\n===== Converged in " << optimizer.iterations()
            << " iterations (" << microseconds << " uS) with final error "
            << optimizer.error() << " ======" << std::endl;

  {
    std::stringstream ss;
    ss << "tag_map_" << result.size() << ".dot";
    graph.saveGraph(ss.str(), result);
  }

  CalResult ret;
  ret.result = result;

  {
    gtsam::Marginals marginals(graph, result);
    std::vector<frc::AprilTag> tags;

    std::cout << "Results:" << std::endl;
    for (auto [key, value] : result) {
      std::cout << "\n========= Key " << gtsam::Symbol(key) << " ==========\n";

      // Assume all our keys are pose3 factors. lol. Print out some fun info
      // about them
      auto est = GtsamToFrcPose3d(result.at<gtsam::Pose3>(key));
      fmt::println("Estimated pose:");
      fmt::println("Translation: x={:.2f} y={:.2f} z={:.2f}", est.X(), est.Y(),
                   est.Z());
      fmt::println("Rotation: W={:.3f} X={:.3f} Y={:.3f} Z={:.3f}",
                   est.Rotation().GetQuaternion().W(),
                   est.Rotation().GetQuaternion().X(),
                   est.Rotation().GetQuaternion().Y(),
                   est.Rotation().GetQuaternion().Z());

      // Covariance is the variance of x_i with x_i - stddev is sqrt(var)
      gtsam::Matrix marginalCov = marginals.marginalCovariance(key);
      auto stddev = marginalCov.diagonal().cwiseSqrt();

      std::cout << "Marginal stddev (r t):" << std::endl << stddev << std::endl;

      // todo - track all tag keys instead of this hack
      if (key >= L(0) && key <= L(1000000)) {
        int id = static_cast<int>(key - L(0));

        tags.push_back(frc::AprilTag{id, est});
        ret.tagPoseCovariances[id] = Pose3WithVariance::FromEigen(est, stddev);
      }
      if (key >= X(0) && key <= X(1000000)) {
        ret.cameraPoseCovariances[key] =
            Pose3WithVariance::FromEigen(est, stddev);
      }
    }

    frc::AprilTagFieldLayout layout{tags, 16.541_m, 8.211_m};
    ret.optimizedLayout = layout;
  }

  return ret;
}
