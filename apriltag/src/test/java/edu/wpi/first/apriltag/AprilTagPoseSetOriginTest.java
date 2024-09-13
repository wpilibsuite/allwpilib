// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class AprilTagPoseSetOriginTest {
  @Test
  void transformationMatches() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(1, Pose3d.kZero),
                new AprilTag(
                    2,
                    new Pose3d(
                        new Translation3d(
                            Units.feetToMeters(4.0), Units.feetToMeters(4), Units.feetToMeters(4)),
                        new Rotation3d(0, 0, Units.degreesToRadians(180))))),
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0));
    layout.setOrigin(AprilTagFieldLayout.OriginPosition.kRedAllianceWallRightSide);

    assertEquals(
        new Pose3d(
            new Translation3d(Units.feetToMeters(54.0), Units.feetToMeters(27.0), 0.0),
            new Rotation3d(0.0, 0.0, Units.degreesToRadians(180.0))),
        layout.getTagPose(1).orElse(null));

    assertEquals(
        new Pose3d(
            new Translation3d(
                Units.feetToMeters(50.0), Units.feetToMeters(23.0), Units.feetToMeters(4)),
            Rotation3d.kZero),
        layout.getTagPose(2).orElse(null));
  }
}
