// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/units/curvature.h"

using namespace wpi::units::literals;

inline static constexpr wpi::units::curvature_t x = 1_rad / 1_m;

static_assert(x.value() == 1.0);
