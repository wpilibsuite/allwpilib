// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fields/fields.h"

#include "fields/2018-powerup.h"
#include "fields/2019-deepspace.h"
#include "fields/2020-infiniterecharge.h"
#include "fields/2021-barrel.h"
#include "fields/2021-bounce.h"
#include "fields/2021-galacticsearcha.h"
#include "fields/2021-galacticsearchb.h"
#include "fields/2021-infiniterecharge.h"
#include "fields/2021-slalom.h"
#include "fields/2022-rapidreact.h"
#include "fields/2023-chargedup.h"
#include "fields/2024-crescendo.h"
#include "fields/2025-reefscape.h"

using namespace fields;

static const Field kFields[] = {
    {"2025 Reefscape", GetResource_2025_reefscape_json,
     GetResource_2025_field_png},
    {"2024 Crescendo", GetResource_2024_crescendo_json,
     GetResource_2024_field_png},
    {"2023 Charged Up", GetResource_2023_chargedup_json,
     GetResource_2023_field_png},
    {"2022 Rapid React", GetResource_2022_rapidreact_json,
     GetResource_2022_field_png},
    {"2021 Barrel Racing Path", GetResource_2021_barrelracingpath_json,
     GetResource_2021_barrel_png},
    {"2021 Bounce Path", GetResource_2021_bouncepath_json,
     GetResource_2021_bounce_png},
    {"2021 Galactic Search A", GetResource_2021_galacticsearcha_json,
     GetResource_2021_galacticsearcha_png},
    {"2021 Galactic Search B", GetResource_2021_galacticsearchb_json,
     GetResource_2021_galacticsearchb_png},
    {"2021 Infinite Recharge", GetResource_2021_infiniterecharge_json,
     GetResource_2021_field_png},
    {"2021 Slalom Path", GetResource_2021_slalompath_json,
     GetResource_2021_slalom_png},
    {"2020 Infinite Recharge", GetResource_2020_infiniterecharge_json,
     GetResource_2020_field_png},
    {"2019 Destination: Deep Space", GetResource_2019_deepspace_json,
     GetResource_2019_field_jpg},
    {"2018 Power Up", GetResource_2018_powerup_json,
     GetResource_2018_field_jpg},
};

std::span<const Field> fields::GetFields() {
  return kFields;
}
