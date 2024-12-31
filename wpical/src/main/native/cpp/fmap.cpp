// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fmap.h"

#include <string>
#include <vector>

wpi::json fmap::singleTag(int tag, const tag::Pose& tagpose) {
  std::vector<double> transform = {};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      transform.push_back(tagpose.transformMatrixFmap(i, j));
    }
  }

  return {{"family", "apriltag3_36h11_classic"},
          {"id", tag},
          {"size", 165.1},
          {"transform", transform},
          {"unique", true}};
}

wpi::json fmap::convertfmap(const wpi::json& json) {
  std::string fmapstart = "{\"fiducials\":[";

  std::string fmapend = "],\"type\":\"frc\"}";

  Fieldmap fieldmap(json);

  for (int i = 0; i < fieldmap.getNumTags(); i++) {
    fmapstart += singleTag(i + 1, fieldmap.getTag(i + 1)).dump();
    if (i != fieldmap.getNumTags() - 1) {
      fmapstart += ",";
    }
  }

  return wpi::json::parse(fmapstart.append(fmapend));
}
