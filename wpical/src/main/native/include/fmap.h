// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <string>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <tagpose.h>

#include "fieldmap.h"

namespace fmap {
std::string singleTag(int tag, tag::pose tagpose, bool endTag);
std::string convertfmap(nlohmann::json json);
}  // namespace fmap
