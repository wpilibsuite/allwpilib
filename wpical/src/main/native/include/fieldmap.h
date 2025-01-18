// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <map>

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
      double tag_id = static_cast<int>(tag.value().at("ID"));
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

      tagMap.emplace(
          tag_id, tag::Pose(tag_id, tagXPos, tagYPos, tagZPos, tagWQuat,
                            tagXQuat, tagYQuat, tagZQuat, field_length_meters,
                            field_width_meters));
    }
    fieldLength = field_length_meters;
    fieldWidth = field_width_meters;
  }

  const tag::Pose& getTag(size_t tag) const { return tagMap.at(tag); }

  int getNumTags() const { return tagMap.size(); }

  bool hasTag(int tag) { return tagMap.find(tag) != tagMap.end(); }

  wpi::json toJson() {
    wpi::json json;
    for (auto& [key, val] : tagMap) {
      json["tags"].push_back(val.toJson());
    }
    json["field"]["length"] = fieldLength;
    json["field"]["width"] = fieldWidth;
    return json;
  }

  static double minimizeAngle(double angle) {
    angle = std::fmod(angle, 360);
    if (angle > 180) {
      return angle - 360;
    } else if (angle < -180) {
      return angle + 360;
    }
    return angle;
  }

  void replaceTag(int tag_id, tag::Pose pose) {
    tagMap.erase(tag_id);
    tagMap.emplace(tag_id, pose);
  }

 private:
  double fieldLength;
  double fieldWidth;
  std::map<int, tag::Pose> tagMap;
};
