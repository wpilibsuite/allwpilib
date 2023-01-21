// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/servo/ActuonixL16R.h"

using namespace frc;

ActuonixL16R::ActuonixL16R(int channel, Stroke stroke)
    : LinearServo("ActuonixL16R", channel, [stroke]() {
        if (stroke == Stroke::_50mm) {
          return 50_mm;
        } else if (stroke == Stroke::_100mm) {
          return 100_mm;
        } else {
          return 140_mm;
        }
      }()) {
  SetBounds(2.0, 1.8, 1.5, 1.2, 1.0);
}
