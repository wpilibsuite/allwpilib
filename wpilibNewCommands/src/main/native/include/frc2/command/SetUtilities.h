// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SmallVector.h>
#include <wpi/span.h>

namespace frc2 {
template <typename T>
void SetInsert(wpi::SmallVectorImpl<T*>& vector, wpi::span<T* const> toAdd) {
  for (auto addCommand : toAdd) {
    bool exists = false;
    for (auto existingCommand : vector) {
      if (addCommand == existingCommand) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      vector.emplace_back(addCommand);
    }
  }
}
}  // namespace frc2
