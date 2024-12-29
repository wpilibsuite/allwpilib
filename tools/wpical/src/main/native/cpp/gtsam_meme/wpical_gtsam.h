// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gtsam/geometry/Cal3_S2.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/linear/NoiseModel.h>
#include <gtsam/nonlinear/Values.h>

#include <map>
#include <utility>
#include <vector>

#include <frc/apriltag/AprilTagFieldLayout.h>

#include "Pose3WithVariance.h"
#include "TagDetection.h"
#include "gtsam_tag_map.h"

namespace wpical {
using KeyframeMap = std::map<gtsam::Key, std::vector<TagDetection>>;

struct CalResult {
  gtsam::Values result;

  std::map<int32_t, Pose3WithVariance> tagPoseCovariances;
  std::map<gtsam::Key, Pose3WithVariance> cameraPoseCovariances;

  frc::AprilTagFieldLayout optimizedLayout;
};

// note that we expect the pixel locations to be -undistorted- here
CalResult OptimizeLayout(
    const GtsamApriltagMap& tagLayoutGuess, KeyframeMap keyframes,
    gtsam::Cal3_S2 cal,
    const std::map<int32_t, std::pair<gtsam::Pose3, gtsam::SharedNoiseModel>>&
        fixedTags,
    const gtsam::SharedNoiseModel cameraNoise);
}  // namespace wpical
