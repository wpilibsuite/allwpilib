// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <vector>

#include <tagpose.h>
#include <wpi/json.h>

class Fieldmap {
 public:
  Fieldmap() = default;
  explicit Fieldmap(const wpi::json& json) {
    double field_length_meters =
        static_cast<double>(json.at("field").at("length"));
    double field_width_meters =
        static_cast<double>(json.at("field").at("width"));
    for (const auto& tag : json.at("tags").items()) {
      double tagXPos =
          static_cast<double>(tag.value().at("pose").at("translation").at("x"));
      double tagYPos =
          static_cast<double>(tag.value().at("pose").at("translation").at("y"));
      double tagZPos =
          static_cast<double>(tag.value().at("pose").at("translation").at("z"));
      double tagWQuat = static_cast<double>(
          tag.value().at("pose").at("rotation").at("quaternion").at("W"));
      double tagXQuat = static_cast<double>(
          tag.value().at("pose").at("rotation").at("quaternion").at("X"));
      double tagYQuat = static_cast<double>(
          tag.value().at("pose").at("rotation").at("quaternion").at("Y"));
      double tagZQuat = static_cast<double>(
          tag.value().at("pose").at("rotation").at("quaternion").at("Z"));

      tagVec.emplace_back(tagXPos, tagYPos, tagZPos, tagWQuat, tagXQuat,
                          tagYQuat, tagZQuat, field_length_meters,
                          field_width_meters);
    }
  }

  const tag::Pose& getTag(size_t tag) const { return tagVec[tag - 1]; }

  int getNumTags() const { return tagVec.size(); }

  static double minimizeAngle(double angle) {
    angle = std::fmod(angle, 360);
    if (angle > 180) {
      return angle - 360;
    } else if (angle < -180) {
      return angle + 360;
    }
    return angle;
  }

 private:
  std::vector<tag::Pose> tagVec;
};
