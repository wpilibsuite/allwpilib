// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fields/fields.h"

#include "fields/frc/2018-powerup.h"
#include "fields/frc/2019-deepspace.h"
#include "fields/frc/2020-infiniterecharge.h"
#include "fields/frc/2021-barrelracingpath.h"
#include "fields/frc/2021-bouncepath.h"
#include "fields/frc/2021-galacticsearcha.h"
#include "fields/frc/2021-galacticsearchb.h"
#include "fields/frc/2021-infiniterecharge.h"
#include "fields/frc/2021-slalompath.h"
#include "fields/frc/2022-rapidreact.h"
#include "fields/frc/2023-chargedup.h"
#include "fields/frc/2024-crescendo.h"
#include "fields/frc/2025-reefscape.h"
#include "fields/ftc/2024-2025-intothedeep.h"

using namespace fields;

static const Field kFields[] = {
    {"2025 FRC Reefscape", GetResource_2025_reefscape_json,
     GetResource_2025_reefscape_png},
    {"2024-2025 FTC Into The Deep", GetResource_2024_2025_intothedeep_json,
     GetResource_2024_2025_intothedeep_png},
    {"2024 FRC Crescendo", GetResource_2024_crescendo_json,
     GetResource_2024_crescendo_png},
    {"2023 FRC Charged Up", GetResource_2023_chargedup_json,
     GetResource_2023_chargedup_png},
    {"2022 FRC Rapid React", GetResource_2022_rapidreact_json,
     GetResource_2022_rapidreact_png},
    {"2021 FRC Barrel Racing Path", GetResource_2021_barrelracingpath_json,
     GetResource_2021_barrelracingpath_png},
    {"2021 FRC Bounce Path", GetResource_2021_bouncepath_json,
     GetResource_2021_bouncepath_png},
    {"2021 FRC Galactic Search A", GetResource_2021_galacticsearcha_json,
     GetResource_2021_galacticsearcha_png},
    {"2021 FRC Galactic Search B", GetResource_2021_galacticsearchb_json,
     GetResource_2021_galacticsearchb_png},
    {"2021 FRC Infinite Recharge", GetResource_2021_infiniterecharge_json,
     GetResource_2021_infiniterecharge_png},
    {"2021 FRC Slalom Path", GetResource_2021_slalompath_json,
     GetResource_2021_slalompath_png},
    {"2020 FRC Infinite Recharge", GetResource_2020_infiniterecharge_json,
     GetResource_2020_infiniterecharge_png},
    {"2019 FRC Destination: Deep Space", GetResource_2019_deepspace_json,
     GetResource_2019_deepspace_jpg},
    {"2018 FRC Power Up", GetResource_2018_powerup_json,
     GetResource_2018_powerup_jpg},
};

std::span<const Field> fields::GetFields() {
  return kFields;
}
