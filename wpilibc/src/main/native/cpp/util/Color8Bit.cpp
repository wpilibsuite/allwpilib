// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/util/Color8Bit.h"

#include <fmt/format.h>

using namespace frc;

std::string Color8Bit::HexString() const {
  return fmt::format("#{:02X}{:02X}{:02X}", red, green, blue);
}
