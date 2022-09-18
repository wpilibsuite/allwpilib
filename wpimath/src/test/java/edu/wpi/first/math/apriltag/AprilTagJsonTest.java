package edu.wpi.first.math.apriltag;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.trajectory.TrajectoryUtil;
import org.junit.jupiter.api.Test;

import java.util.Map;

public class AprilTagJsonTest {
    @Test
    void deserializeMatches() {
        var layout = new AprilTagFieldLayout(Map.of(
                1, new Pose3d(0, 0, 0, new Rotation3d(0, 0, 0)),
                3, new Pose3d(0, 1, 0, new Rotation3d(0, 0, 0))
        ));

        var deserialized = assertDoesNotThrow(
                () -> AprilTagUtil.deserializeAprilTagFieldLayout(
                        AprilTagUtil.serializeAprilTagFieldLayout(layout)
                )
        );

        assertEquals(layout.getTags(), deserialized.getTags());
    }
}
