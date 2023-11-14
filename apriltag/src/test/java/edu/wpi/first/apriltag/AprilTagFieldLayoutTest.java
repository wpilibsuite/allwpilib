package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;

public class AprilTagFieldLayoutTest {
  @Test
  void protobufTest() {
    var layout = new AprilTagFieldLayout(List.of(), 12.3, 4.2);
    var packedLayout = AprilTagFieldLayout.proto.createMessage();
    AprilTagFieldLayout.proto.pack(packedLayout, layout);
    var unpackedLayout = AprilTagFieldLayout.proto.unpack(packedLayout);
    assertEquals(layout, unpackedLayout);
  }
}
