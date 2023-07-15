// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {
enum class EdgeConfiguration {
  kNone = 0,
  kRisingEdge = 0x1,
  kFallingEdge = 0x2,
  kBoth = 0x3
};
}  // namespace frc
