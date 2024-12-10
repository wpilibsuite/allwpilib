// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fieldmap.h>

#include <string>

#include <opencv2/opencv.hpp>
#include <tagpose.h>
#include <wpi/json.h>

namespace fmap {
std::string singleTag(int tag, tag::pose tagpose, bool endTag);
std::string convertfmap(wpi::json json);
}  // namespace fmap
