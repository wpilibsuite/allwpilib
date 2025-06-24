// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "fields/fields.h"

// TODO(crueter): This *could* be moved into separate frc/ftc directories
// same for the resources themselves. Would make naming a bit easier perhaps?
// but we'd have to mess with gradle's generated functions and such.
// Might not even be worth it.

#include "fields/2018-frc-powerup.h"
#include "fields/2019-frc-deepspace.h"
#include "fields/2020-frc-infiniterecharge.h"
#include "fields/2021-frc-barrelracingpath.h"
#include "fields/2021-frc-bouncepath.h"
#include "fields/2021-frc-galacticsearcha.h"
#include "fields/2021-frc-galacticsearchb.h"
#include "fields/2021-frc-infiniterecharge.h"
#include "fields/2021-frc-slalompath.h"
#include "fields/2022-frc-rapidreact.h"
#include "fields/2023-frc-chargedup.h"
#include "fields/2024-frc-crescendo.h"
#include "fields/2025-frc-reefscape.h"

#include "fields/2024-2025-ftc-intothedeep.h"

using namespace fields;

// TODO(crueter): namespace this somehow

static const Field kFields[] = {
    {"2025 FRC Reefscape", GetResource_2025_frc_reefscape_json,
     GetResource_2025_frc_reefscape_png},
    {"2024-2025 FTC Into The Deep", GetResource_2024_2025_ftc_intothedeep_json,
     GetResource_2024_2025_ftc_intothedeep_png},
    {"2024 FRC Crescendo", GetResource_2024_frc_crescendo_json,
     GetResource_2024_frc_crescendo_png},
    {"2023 FRC Charged Up", GetResource_2023_frc_chargedup_json,
     GetResource_2023_frc_chargedup_png},
    {"2022 FRC Rapid React", GetResource_2022_frc_rapidreact_json,
     GetResource_2022_frc_rapidreact_png},
    {"2021 FRC Barrel Racing Path", GetResource_2021_frc_barrelracingpath_json,
     GetResource_2021_frc_barrelracingpath_png},
    {"2021 FRC Bounce Path", GetResource_2021_frc_bouncepath_json,
     GetResource_2021_frc_bouncepath_png},
    {"2021 FRC Galactic Search A", GetResource_2021_frc_galacticsearcha_json,
     GetResource_2021_frc_galacticsearcha_png},
    {"2021 FRC Galactic Search B", GetResource_2021_frc_galacticsearchb_json,
     GetResource_2021_frc_galacticsearchb_png},
    {"2021 FRC Infinite Recharge", GetResource_2021_frc_infiniterecharge_json,
     GetResource_2021_frc_infiniterecharge_png},
    {"2021 FRC Slalom Path", GetResource_2021_frc_slalompath_json,
     GetResource_2021_frc_slalompath_png},
    {"2020 FRC Infinite Recharge", GetResource_2020_frc_infiniterecharge_json,
     GetResource_2020_frc_infiniterecharge_png},
    {"2019 FRC Destination: Deep Space", GetResource_2019_frc_deepspace_json,
     GetResource_2019_frc_deepspace_jpg},
    {"2018 FRC Power Up", GetResource_2018_frc_powerup_json,
     GetResource_2018_frc_powerup_jpg},
};

std::span<const Field> fields::GetFields() {
  return kFields;
}
