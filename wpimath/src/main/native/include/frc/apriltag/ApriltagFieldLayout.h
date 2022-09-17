// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Transform2d.h"
#include "units/acceleration.h"
#include "units/curvature.h"
#include "units/time.h"
#include "units/velocity.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {
class WPILIB_DLLEXPORT ApriltagFieldLayout {
 public:
    struct WPILIB_DLLEXPORT Apriltag {
        int id;

        Pose3d pose;
    };

    ApriltagFieldLayout() = default;
    
    explicit ApriltagFieldLayout(const std::vector<Apriltag>& apriltags);
    
    const std::vector<Apriltag>& GetTags() const { return m_apriltags; };
    
    Pose3d GetTagPose(int id) const;
    
    void setShouldMirror(bool mirror) { m_mirror = mirror; }
    
 private:
    std::vector<Apriltag> m_apriltags;
    bool m_mirror;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const ApriltagFieldLayout::Apriltag& apriltag);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, ApriltagFieldLayout::Apriltag& apriltag);
}