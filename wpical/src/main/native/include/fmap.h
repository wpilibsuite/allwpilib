// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPIcal license file in the wpical directory of this project.

#pragma once

#include <fieldmap.h>

#include <tagpose.h>
#include <wpi/json.h>

namespace fmap {
wpi::json singleTag(int tag, const tag::Pose& tagpose);
wpi::json convertfmap(const wpi::json& json);
}  // namespace fmap
