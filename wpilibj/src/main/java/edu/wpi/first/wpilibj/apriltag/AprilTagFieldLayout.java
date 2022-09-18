package edu.wpi.first.wpilibj.apriltag;

import edu.wpi.first.math.WPIMathJNI;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.DriverStation;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

public class AprilTagFieldLayout {
    private final Map<Integer, Pose3d> m_tags = new HashMap<>();
    private boolean m_mirror = false;

    public AprilTagFieldLayout(String path) throws IOException {
        this(Path.of(path));
    }

    public AprilTagFieldLayout(Path path) throws IOException {
        this(AprilTagUtil.createAprilTagFieldLayoutFromElements(WPIMathJNI.deserializeAprilTagLayout(Files.readString(path))));
    }

    public AprilTagFieldLayout(Map<Integer, Pose3d> tags) {
        // To ensure the underlying semantics don't change with what kind of map is passed in
        m_tags.putAll(tags);
    }

    public void setAlliance(DriverStation.Alliance alliance) {
        m_mirror = alliance == DriverStation.Alliance.Red;
    }

    public Map<Integer, Pose3d> getTags() {
        return m_tags;
    }

    public Pose3d getTag(int id) {
        Pose3d tag = m_tags.get(id);
        if(m_mirror) {
            tag = tag.relativeTo(new Pose3d(new Translation3d(Units.feetToMeters(54.0), Units.feetToMeters(27.0), 0.0), new Rotation3d(0.0, 0.0, 180.0)));
        }
        return tag;
    }

    public String serialize() {
        return WPIMathJNI.serializeAprilTagLayout(AprilTagUtil.getElementsFromAprilTagFieldLayout(m_tags));
    }
}
