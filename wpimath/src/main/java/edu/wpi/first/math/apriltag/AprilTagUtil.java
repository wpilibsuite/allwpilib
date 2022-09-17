package edu.wpi.first.math.apriltag;

import edu.wpi.first.math.WPIMathJNI;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;

import java.util.HashMap;
import java.util.Map;

public class AprilTagUtil {
    /**
     * Creates an AprilTag layout from a double[] of elements.
     *
     * @param elements A double[] containing the raw elements of the AprilTag layout.
     * @return An AprilTag layout created from the elements.
     */
    private static AprilTagFieldLayout createAprilTagFieldLayoutFromElements(double[] elements) {
        // Make sure that the elements have the correct length.
        if (elements.length % 7 != 0) {
            throw new AprilTagLayoutSerializationException(
                    "An error occurred when converting trajectory elements into a trajectory.");
        }

        // Create a list of states from the elements.
        Map<Integer, Pose3d> apriltagLayout = new HashMap<>();
        for (int i = 0; i < elements.length; i += 7) {
            apriltagLayout.put(
                    (int) elements[i],
                    new Pose3d(
                            elements[i + 1],
                            elements[i + 2],
                            elements[i + 3],
                            new Rotation3d(new Quaternion(
                                    elements[i + 4],
                                    elements[i + 5],
                                    elements[i + 6],
                                    elements[i + 7]
                            ))
                    )
            );
        }
        return new AprilTagFieldLayout(apriltagLayout);
    }

    /**
     * Returns a double[] of elements from the given AprilTag layout.
     *
     * @param aprilTagFieldLayout The AprilTag field layout to retrieve raw elements from.
     * @return A double[] of elements from the given trajectory.
     */
    private static double[] getElementsFromAprilTagFieldLayout(AprilTagFieldLayout aprilTagFieldLayout) {
        // Create a double[] of elements and fill it from the trajectory states.
        double[] elements = new double[aprilTagFieldLayout.getTags().size() * 7];

        for (int i = 0; i < aprilTagFieldLayout.getTags().size() * 7; i += 7) {
            var entry = aprilTagFieldLayout.getTags().entrySet().stream().toList().get(i / 7);
            elements[i] = entry.getKey();
            elements[i + 1] = entry.getValue().getX();
            elements[i + 2] = entry.getValue().getY();
            elements[i + 3] = entry.getValue().getZ();
            elements[i + 4] = entry.getValue().getRotation().getQuaternion().getW();
            elements[i + 5] = entry.getValue().getRotation().getQuaternion().getX();
            elements[i + 6] = entry.getValue().getRotation().getQuaternion().getY();
            elements[i + 7] = entry.getValue().getRotation().getQuaternion().getZ();
        }
        return elements;
    }

    public static AprilTagFieldLayout deserializeAprilTagFieldLayout(String json) {
        return createAprilTagFieldLayoutFromElements(WPIMathJNI.deserializeAprilTagLayout(json));
    }

    public static String serializeAprilTagFieldLayout(AprilTagFieldLayout aprilTagFieldLayout) {
        return WPIMathJNI.serializeAprilTagLayout(getElementsFromAprilTagFieldLayout(aprilTagFieldLayout));
    }

    public static class AprilTagLayoutSerializationException extends RuntimeException {
        public AprilTagLayoutSerializationException(String message) {
            super(message);
        }
    }
}
