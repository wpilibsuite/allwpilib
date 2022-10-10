// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/util/Color.h"

#include <fmt/format.h>

using namespace frc;

std::string Color::HexString() const {
  return fmt::format("#{:02X}{:02X}{:02X}", static_cast<int>(255.0 * red),
                     static_cast<int>(255.0 * green),
                     static_cast<int>(255.0 * blue));
}
