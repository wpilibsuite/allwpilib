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
class WPILIB_DLLEXPORT AprilTagFieldLayout {
 public:
    struct WPILIB_DLLEXPORT AprilTag {
        int id;

        Pose3d pose;
        
        /**
         * Checks equality between this State and another object.
         *
         * @param other The other object.
         * @return Whether the two objects are equal.
         */
        bool operator==(const AprilTag& other) const;

        /**
         * Checks inequality between this State and another object.
         *
         * @param other The other object.
         * @return Whether the two objects are not equal.
         */
        bool operator!=(const AprilTag& other) const;

    };

    AprilTagFieldLayout() = default;
    
    explicit AprilTagFieldLayout(const std::vector<AprilTag>& apriltags);
    
    const std::vector<AprilTag>& GetTags() const { return m_apriltags; };
    
    Pose3d GetTagPose(int id) const;
    
    void SetShouldMirror(bool mirror);
    
 private:
    std::vector<AprilTag> m_apriltags;
    bool m_mirror;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const AprilTagFieldLayout::AprilTag& apriltag);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, AprilTagFieldLayout::AprilTag& apriltag);
}