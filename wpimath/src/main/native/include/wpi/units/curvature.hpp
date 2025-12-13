// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/angle.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/length.hpp"

namespace wpi::units {
using curvature_t = wpi::units::unit_t<wpi::units::compound_unit<
    wpi::units::radians, wpi::units::inverse<wpi::units::meters>>>;
}  // namespace wpi::units
