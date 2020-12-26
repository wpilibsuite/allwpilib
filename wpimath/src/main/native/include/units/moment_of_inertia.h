// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/area.h"
#include "units/base.h"
#include "units/mass.h"

namespace units {
UNIT_ADD(moment_of_inertia, kilogram_square_meter, kilogram_square_meters,
         kg_sq_m, compound_unit<mass::kilograms, area::square_meters>)

using namespace moment_of_inertia;
}  // namespace units
