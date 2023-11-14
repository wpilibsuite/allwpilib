package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import org.junit.jupiter.api.Test;

public class AprilTagTest {
  @Test
  void protobufTest() {
    var tag = new AprilTag(0, new Pose3d());
    var packedTag = AprilTag.proto.createMessage();
    AprilTag.proto.pack(packedTag, tag);
    var unpackedTag = AprilTag.proto.unpack(packedTag);
    assertEquals(tag, unpackedTag);
  }
}
