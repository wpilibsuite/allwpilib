package edu.wpi.first.math.kinematics;

public class HDriveWheelSpeeds {
    public double leftMetersPerSecond;
    public double rightMetersPerSecond;
    public double lateralMetersPerSecond;

    /** Constructs a HDriveWheelSpeeds with zeros for left, right, and lateral speeds. */
    public HDriveWheelSpeeds() {}

    public HDriveWheelSpeeds(double leftMetersPerSecond, double rightMetersPerSecond, double lateralMetersPerSecond) {
        this.leftMetersPerSecond = leftMetersPerSecond;
        this.rightMetersPerSecond = rightMetersPerSecond;
        this.lateralMetersPerSecond = lateralMetersPerSecond;
    }
}
