// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fmap.hpp"

#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>

#include "wpi/apriltag/AprilTagFieldLayout.hpp"

using namespace fmap;

Fieldmap::Fieldmap(std::string type, std::vector<Fiducial> fiducials)
    : type(std::move(type)), fiducials(std::move(fiducials)) {}

Fieldmap::Fieldmap(const wpi::apriltag::AprilTagFieldLayout& layout)
    : type("frc") {
  // https://docs.limelightvision.io/docs/docs-limelight/pipeline-apriltag/apriltag-map-specification
  for (auto tag : layout.GetTags()) {
    // TODO: remove variable when clang 16 is available on Mac
    Fiducial fiducial{"apriltag3_36h11_classic", tag.ID, 165.1,
                      tag.pose.ToMatrix(), 1};
    fiducials.emplace_back(fiducial);
  }
}

void fmap::to_json(wpi::util::json& json, const Fiducial& layout) {
  json = wpi::util::json::object();
  json["family"] = layout.family;
  json["id"] = layout.id;
  json["size"] = layout.size;
  json["transform"] =
      std::vector<double>{layout.transform.data(),
                          layout.transform.data() + layout.transform.size()};
  json["unique"] = layout.unique;
}

void fmap::from_json(const wpi::util::json& json, Fiducial& layout) {
  auto& arr = json.at("transform").get_array();
  std::vector<double> vec;
  vec.reserve(arr.size());
  for (auto& elem : arr) {
    vec.push_back(elem.get_number());
  }
  layout = {json.at("family").get_string(),
            static_cast<int>(json.at("id").get_int()),
            json.at("size").get_number(), Eigen::Matrix4d{vec.data()},
            static_cast<int>(json.at("unique").get_int())};
}

void fmap::to_json(wpi::util::json& json, const Fieldmap& layout) {
  json = wpi::util::json::object("type", "frc", "fiducials", layout.fiducials);
}

void fmap::from_json(const wpi::util::json& json, Fieldmap& layout) {
  auto& arr = json.at("fiducials").get_array();
  std::vector<Fiducial> fiducials;
  fiducials.reserve(arr.size());
  for (auto& elem : arr) {
    fiducials.emplace_back(elem.get<Fiducial>());
  }
  layout = {json.at("type").get_string(), fiducials};
}
