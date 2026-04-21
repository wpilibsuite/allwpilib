// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/fields.hpp"

#include "wpi/fields/frc/2018-powerup.hpp"
#include "wpi/fields/frc/2019-deepspace.hpp"
#include "wpi/fields/frc/2020-infiniterecharge.hpp"
#include "wpi/fields/frc/2021-barrelracingpath.hpp"
#include "wpi/fields/frc/2021-bouncepath.hpp"
#include "wpi/fields/frc/2021-galacticsearcha.hpp"
#include "wpi/fields/frc/2021-galacticsearchb.hpp"
#include "wpi/fields/frc/2021-infiniterecharge.hpp"
#include "wpi/fields/frc/2021-slalompath.hpp"
#include "wpi/fields/frc/2022-rapidreact.hpp"
#include "wpi/fields/frc/2023-chargedup.hpp"
#include "wpi/fields/frc/2024-crescendo.hpp"
#include "wpi/fields/frc/2025-reefscape.hpp"
#include "wpi/fields/frc/2026-rebuilt.hpp"
#include "wpi/fields/ftc/2024-2025-intothedeep.hpp"
#include "wpi/fields/ftc/2025-2026-decode.hpp"

using namespace wpi::fields;

static const Field kFields[] = {
    {"2026 FRC Rebuilt", GetResource_2026_rebuilt_json,
     GetResource_2026_rebuilt_png},
    {"2025-2026 FTC DECODE", GetResource_2025_2026_decode_json,
     GetResource_2025_2026_decode_png},
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

std::span<const Field> wpi::fields::GetFields() {
  return kFields;
}
