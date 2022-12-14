package edu.wpi.first.math.util;

public final class FalconUnitConversion {
    private static final double kFalconIntegratedEncoderResolution = 2048;

    /** Utility class, so constructor is private. */
    private FalconUnitConversion() {
        throw new UnsupportedOperationException("This is a utility class!");
    }

    public static double falcon500VelocityToAngularVelocity(double falconVelocity, double gearRatio) {
        return Units.rpmToAngularVelocity(falcon500VelocityToRPM(falconVelocity)) / gearRatio;
    }

    public static double falcon500VelocityToAngularVelocity(double falconVelocity) {
        return falcon500VelocityToAngularVelocity(falconVelocity, 1);
    }

    public static double angularVelocityToFalcon500Velocity(double angularVelocity, double gearRatio) {
        return rpmToFalcon500Velocity(Units.angularVelocityToRPM(angularVelocity)) * gearRatio;
    }

    public static double angularVelocityToFalcon500Velocity(double angularVelocity) {
        return angularVelocityToFalcon500Velocity(angularVelocity, 1);
    }

    public static double falcon500VelocityToLinearVelocity(double falconVelocity, double radius, double gearRatio) {
        return Units.rpmToLinearVelocity(falcon500VelocityToRPM(falconVelocity), radius) / gearRatio;
    }

    public static double falcon500VelocityToLinearVelocity(double falconVelocity, double radius) {
        return falcon500VelocityToLinearVelocity(falconVelocity, radius, 1);
    }

    public static double linearVelocityToFalcon500Velocity(double linearVelocity, double radius, double gearRatio) {
        return rpmToFalcon500Velocity(Units.linearVelocityToRPM(linearVelocity, radius)) * gearRatio;
    }

    public static double linearVelocityToFalcon500Velocity(double linearVelocity, double radius) {
        return linearVelocityToFalcon500Velocity(linearVelocity, radius, 1);
    }

    public static double falcon500VelocityToRPM(double falconVelocity) {
        return falconVelocity * 600.0 / kFalconIntegratedEncoderResolution;
    }

    public static double rpmToFalcon500Velocity(double rpm) {
        return rpm * kFalconIntegratedEncoderResolution / 600.0;
    }

    public static double falcon500PositionToDegrees(double falconPosition) {
        return (falconPosition / kFalconIntegratedEncoderResolution) * 360;
    }

    public static double falcon500PositionToRadians(double falconPosition) {
        return Math.toRadians(falcon500PositionToDegrees(falconPosition));
    }

    public static double degreesToFalcon500Position(double positionDegrees) {
        double optimizedPosition = positionDegrees % 360;
        return (optimizedPosition / 360.0) * kFalconIntegratedEncoderResolution;
    }

    public static double radiansToFalcon500Position(double positionRadians) {
        double optimizedPosition = positionRadians % (2 * Math.PI);
        return degreesToFalcon500Position(Math.toDegrees(optimizedPosition));
    }
}
