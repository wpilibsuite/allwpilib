// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "fieldmap.hpp"
#include "tagpose.hpp"
#include "wpi/util/json.hpp"

namespace fmap {
wpi::util::json singleTag(int tag, const tag::Pose& tagpose);
wpi::util::json convertfmap(const wpi::util::json& json);
}  // namespace fmap
