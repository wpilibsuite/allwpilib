// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/angle.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/length.hpp"

namespace units {
using curvature_t = units::unit_t<
    units::compound_unit<units::radians, units::inverse<units::meters>>>;
}  // namespace units
