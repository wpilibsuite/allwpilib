/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/ArrayRef.h>
#include <wpi/SmallVector.h>

namespace frc2 {
template <typename T>
void SetInsert(wpi::SmallVectorImpl<T*>& vector, wpi::ArrayRef<T*> toAdd) {
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
