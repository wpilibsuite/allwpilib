// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagDetector.h"

int main() {
  frc::AprilTagDetector detector;
  detector.AddFamily("tag16h5");
  detector.SetConfig({.refineEdges = false});
}
