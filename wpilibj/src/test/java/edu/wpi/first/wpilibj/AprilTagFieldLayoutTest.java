package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.WPIMathJNI;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.wpilibj.apriltag.AprilTagFieldLayout;
import edu.wpi.first.wpilibj.apriltag.AprilTagUtil;
import java.util.Map;
import org.junit.jupiter.api.Test;

class AprilTagFieldLayoutTest {
  @Test
  void deserializeMatches() {
    var layout =
        new AprilTagFieldLayout(
            Map.of(
                1, new Pose3d(0, 0, 0, new Rotation3d(0, 0, 0)),
                3, new Pose3d(0, 1, 0, new Rotation3d(0, 0, 0))));

    var deserialized =
        assertDoesNotThrow(
            () ->
                new AprilTagFieldLayout(
                    AprilTagUtil.createAprilTagFieldLayoutFromElements(
                        WPIMathJNI.deserializeAprilTagLayout(layout.serialize()))));

    assertEquals(layout.getTags(), deserialized.getTags());
  }
}
