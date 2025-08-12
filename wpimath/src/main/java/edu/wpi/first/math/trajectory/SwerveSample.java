package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.*;
import edu.wpi.first.units.measure.Time;

public class SwerveSample extends TrajectorySample<SwerveSample> {
    private final SwerveDriveKinematics kinematics;

    /**
     * The states of the wheels in the robot's reference frame,
     * in the order that each wheel is specified in the
     * {@link SwerveDriveKinematics} object.
     */
    public final SwerveModuleState[] states;

    /**
     * Constructs a SwerveSample.
     * 
     * @param timestamp The timestamp of the sample.
     * @param pose The robot pose at this sample (in the field reference frame).
     * @param vel The robot velocity at this sample (in the robot's reference frame).
     * @param accel The robot acceleration at this sample (in the robot's reference frame).
     * @param kinematics The kinematics of the drivetrain.
     */
    public SwerveSample(
            Time timestamp, 
            Pose2d pose, 
            ChassisSpeeds vel, 
            ChassisAccelerations accel, 
            SwerveDriveKinematics kinematics) {
        super(timestamp, pose, vel, accel);
        this.kinematics = kinematics;
        
        states = kinematics.toWheelSpeeds(vel);
    }

    /**
     * Constructs a SwerveSample from another TrajectorySample.
     * 
     * @param sample The TrajectorySample to copy.
     * @param kinematics The kinematics of the drivetrain.
     */
    public SwerveSample(TrajectorySample<?> sample, SwerveDriveKinematics kinematics) {
        this(sample.timestamp, sample.pose, sample.vel, sample.accel, kinematics);   
    }

    /**
     * Interpolates between this sample and the given sample using Euler integration
     * on this sample state.
     *
     * @param endValue The end sample.
     * @param t The time between this sample and the end sample. Should be in the range [0, 1].
     * @return new sample
     */
    @Override
    public SwerveSample interpolate(SwerveSample endValue, double t) {
        return new SwerveSample(TrajectorySample.kinematicInterpolate(this, endValue, t), kinematics);
    }
}
