package edu.wpi.first.math.apriltag;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;

import java.util.HashMap;
import java.util.Map;

public class ApriltagFieldLayout {
    private final Map<Integer, Pose3d> m_tags = new HashMap<>();
    private boolean m_mirror = false;

    public ApriltagFieldLayout(Map<Integer, Pose3d> tags) {
        // To ensure the underlying semantics don't change with what kind of map is passed in
        m_tags.putAll(tags);
    }

    public void setShouldMirror(boolean mirror) {
        m_mirror = mirror;
    }

    public Map<Integer, Pose3d> getTags() {
        return m_tags;
    }

    public Pose3d getTag(int id) {
        Pose3d tag = m_tags.get(id);
        if(m_mirror) {
            tag = tag.relativeTo(new Pose3d(new Translation3d(16.4592, 8.2296, 0), new Rotation3d(0, 0, 180)));
        }
        return tag;
    }
}
