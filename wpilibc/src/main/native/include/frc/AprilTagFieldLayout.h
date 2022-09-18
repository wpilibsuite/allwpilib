// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/DriverStation.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Transform2d.h"
#include "units/acceleration.h"
#include "units/curvature.h"
#include "units/time.h"
#include "units/velocity.h"
#include "frc/apriltag/AprilTagUtil.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
    AprilTagFieldLayout() = default;
    
    explicit AprilTagFieldLayout(const std::vector<AprilTagUtil::AprilTag>& apriltags);
    
    const std::vector<AprilTagUtil::AprilTag>& GetTags() const;
    
    Pose3d GetTagPose(int id) const;
    
    void SetAlliance(DriverStation::Alliance alliance);
    
 private:
    std::vector<AprilTagUtil::AprilTag> m_apriltags;
    bool m_mirror;
};
}