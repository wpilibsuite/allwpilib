// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fmap.h"

#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <frc/apriltag/AprilTagFieldLayout.h>

using namespace fmap;

Fieldmap::Fieldmap(std::string type, std::vector<Fiducial> fiducials)
    : type(std::move(type)), fiducials(std::move(fiducials)) {}

Fieldmap::Fieldmap(const frc::AprilTagFieldLayout& layout) : type("frc") {
  // https://docs.limelightvision.io/docs/docs-limelight/pipeline-apriltag/apriltag-map-specification
  for (auto tag : layout.GetTags()) {
    fiducials.emplace_back("apriltag3_36h11_classic", tag.ID, 165.1,
                           tag.pose.ToMatrix(), 1);
  }
}

void fmap::to_json(wpi::json& json, const Fiducial& layout) {
  json = {{"family", layout.family},
          {"id", layout.id},
          {"size", layout.size},
          {"transform", std::vector<double>{layout.transform.data(),
                                            layout.transform.data() +
                                                layout.transform.size()}},
          {"unique", layout.unique}};
}

void fmap::from_json(const wpi::json& json, Fiducial& layout) {
  auto vec = json.at("transform").get<std::vector<double>>();
  layout = {json.at("family"), json.at("id"), json.at("size"),
            Eigen::Matrix4d{vec.data()}, json.at("unique")};
}

void fmap::to_json(wpi::json& json, const Fieldmap& layout) {
  json = {{"type", "frc"}, {"fiducials", layout.fiducials}};
}

void fmap::from_json(const wpi::json& json, Fieldmap& layout) {
  layout = {json.at("type"), json.at("fiducials")};
}
