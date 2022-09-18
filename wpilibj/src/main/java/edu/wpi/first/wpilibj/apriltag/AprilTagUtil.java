package edu.wpi.first.wpilibj.apriltag;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class AprilTagUtil {
    /**
     * Creates an AprilTag layout from a double[] of elements.
     *
     * @param elements A double[] containing the raw elements of the AprilTag layout.
     * @return An AprilTag layout created from the elements.
     */
    public static Map<Integer, Pose3d> createAprilTagFieldLayoutFromElements(double[] elements) {
        // Make sure that the elements have the correct length.
        if (elements.length % 8 != 0) {
            throw new AprilTagLayoutSerializationException(
                    "An error occurred when converting AprilTag elements into a AprilTag layout.");
        }

        // Create a list of states from the elements.
        Map<Integer, Pose3d> apriltagLayout = new HashMap<>();
        for (int i = 0; i < elements.length; i += 8) {
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
        return apriltagLayout;
    }

    /**
     * Returns a double[] of elements from the given AprilTag layout.
     *
     * @param aprilTagLayout The AprilTag layout to retrieve raw elements from.
     * @return A double[] of elements from the given AprilTag layout.
     */
     public static double[] getElementsFromAprilTagFieldLayout(Map<Integer, Pose3d> aprilTagLayout) {
        // Create a double[] of elements and fill it from the trajectory states.
        double[] elements = new double[aprilTagLayout.size() * 8];

        ArrayList<Map.Entry<Integer, Pose3d>> entries = new ArrayList<>(aprilTagLayout.entrySet());
        for (int i = 0; i < aprilTagLayout.size() * 8; i += 8) {
            var entry = entries.get(i / 8);
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

    public static class AprilTagLayoutSerializationException extends RuntimeException {
        public AprilTagLayoutSerializationException(String message) {
            super(message);
        }
    }
}
