// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import java.util.List;
import org.junit.jupiter.api.Test;

public class AprilTagFieldLayoutTest {
  @Test
  void protobufTest() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(0, new Pose3d(0, 1, 1, new Rotation3d())),
                new AprilTag(1, new Pose3d(2, 3, 1, new Rotation3d()))),
            12.3,
            4.2);
    var packedLayout = AprilTagFieldLayout.proto.createMessage();
    AprilTagFieldLayout.proto.pack(packedLayout, layout);
    var unpackedLayout = AprilTagFieldLayout.proto.unpack(packedLayout);
    assertEquals(layout, unpackedLayout);
  }
}
