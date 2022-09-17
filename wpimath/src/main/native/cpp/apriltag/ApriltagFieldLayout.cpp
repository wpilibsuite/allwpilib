// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/ApriltagFieldLayout.h"

#include <wpi/json.h>
#include "frc/geometry/Pose3d.h"
#include "units/angle.h"
#include "units/length.h"

using namespace frc;

frc::Pose3d ApriltagFieldLayout::GetTagPose(int id) const {
    Pose3d returnPose;
    for(auto& tag : m_apriltags) {
        if(tag.id == id) {
            returnPose = tag.pose;
        }
    }
    if(m_mirror) {
        returnPose = returnPose.RelativeTo(Pose3d{54_ft, 27_ft, 0_ft, Rotation3d{0_deg, 0_deg, 180_deg}});
    }
    return returnPose;
}

void ApriltagFieldLayout::SetShouldMirror(bool mirror) {
    m_mirror = mirror;
}

void frc::to_json(wpi::json& json, const ApriltagFieldLayout::Apriltag& apriltag) {
    json = wpi::json{
        {"id", apriltag.id},
        {"pose", apriltag.pose}
    };
}

void frc::from_json(const wpi::json& json, ApriltagFieldLayout::Apriltag& apriltag) {
    apriltag.id = json.at("id").get<int>();
    apriltag.pose = json.at("pose").get<Pose3d>();
}