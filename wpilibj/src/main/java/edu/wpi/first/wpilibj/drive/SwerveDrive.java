// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static edu.wpi.first.units.Units.Radians;
import edu.wpi.first.math.Matrix;
import java.util.function.Supplier;
import edu.wpi.first.math.estimator.SwerveDrivePoseEstimator;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveOdometry;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.units.measure.Angle;

/**
 * Represents a complete swerve drive subsystem, capable of controlling modules,
 * updating odometry, and creating estimators for robot pose.
 */
public class SwerveDrive {
    private final SwerveDriveKinematics m_kinematics;
    private final Supplier<Angle> m_gyro;
    private final SwerveDriveOdometry m_odometry;
    private final SwerveModule[] m_modules;

    /**
     * Constructs a SwerveDrive using the given gyro supplier and modules.
     *
     * @param gyroSupplier A supplier that returns the current heading angle of the robot.
     * @param modules The swerve modules used by the drivetrain.
     */
    public SwerveDrive(Supplier<Angle> gyroSupplier, SwerveModule... modules) {
        m_kinematics = SwerveDrive.createKinematics(modules);
        m_odometry = SwerveDrive.createOdometry(gyroSupplier, modules);
        m_gyro = gyroSupplier;
        m_modules = modules;
    }

    /**
     * Constructs a SwerveDrive with a preconfigured odometry object.
     *
     * @param odometry The odometry instance to use.
     * @param gyroSupplier A supplier that returns the current heading angle of the robot.
     * @param modules The swerve modules used by the drivetrain.
     */
    public SwerveDrive(SwerveDriveOdometry odometry, Supplier<Angle> gyroSupplier, SwerveModule... modules) {
        m_kinematics = SwerveDrive.createKinematics(modules);
        m_odometry = odometry;
        m_gyro = gyroSupplier;
        m_modules = modules;
    }


    /**
     * Applies the desired chassis speeds to the swerve modules.
     *
     * @param speeds The desired chassis speeds.
     */
    public void applyChassisSpeeds(ChassisSpeeds speeds) {
        applyStates(m_kinematics.toWheelSpeeds(speeds));
    }

    /**
     * Applies the desired states to each swerve module.
     *
     * @param states The desired states for each module.
     */
    public void applyStates(SwerveModuleState[] states) {
        for (int i = 0; i < m_modules.length; i++) {
            m_modules[i].applyState(states[i]);
        }
    }

    /**
     * Updates the odometry with the current module positions and gyro heading.
     */
    public void updateOdometry() {
        m_odometry.update(Rotation2d.fromRadians(m_gyro.get().in(Radians)), getModulePositions());
    }

    /**
     * Returns the current estimated pose of the robot.
     *
     * @return The current pose in meters.
     */
    public Pose2d getPoseMeters() {
        return m_odometry.getPoseMeters();
    }

    /**
     * Resets the robot pose in odometry.
     *
     * @param newPose The pose to reset to.
     */
    public void resetPose(Pose2d newPose) {
        m_odometry.resetPose(newPose);
    }

    /**
     * Gets the current positions of all swerve modules.
     *
     * @return An array of {@link SwerveModulePosition} for each module.
     */
    public SwerveModulePosition[] getModulePositions() {
        return SwerveDrive.getModulePositions(m_modules);
    }

    /**
     * Creates a kinematics object from the swerve modules.
     *
     * @param modules The modules used in the drivetrain.
     * @return A new {@link SwerveDriveKinematics} instance.
     */
    public static SwerveDriveKinematics createKinematics(SwerveModule... modules) {
        Translation2d[] positions = new Translation2d[modules.length];
        for (int i = 0; i < positions.length; i++) {
            positions[i] = modules[i].getModuleLocation();
        }
        return new SwerveDriveKinematics(positions);
    }

    /**
     * Gets the current positions of the given swerve modules.
     *
     * @param modules The swerve modules.
     * @return An array of {@link SwerveModulePosition}.
     */
    public static SwerveModulePosition[] getModulePositions(SwerveModule... modules) {
        SwerveModulePosition[] positions = new SwerveModulePosition[modules.length];
        for (int i = 0; i < positions.length; i++) {
            positions[i] = modules[i].getPosition();
        }
        return positions;
    }

    /**
     * Creates a new odometry object using the supplied modules and gyro.
     *
     * @param gyro The gyro supplier.
     * @param modules The swerve modules.
     * @return A new {@link SwerveDriveOdometry} instance.
     */
    public static SwerveDriveOdometry createOdometry(Supplier<Angle> gyro, SwerveModule... modules) {
        return new SwerveDriveOdometry(
            createKinematics(modules),
            Rotation2d.fromRadians(gyro.get().in(Radians)),
            getModulePositions(modules)
        );
    }

    /**
     * Creates a new odometry object with a specified initial pose.
     *
     * @param gyro The gyro supplier.
     * @param initialPose The initial pose to start from.
     * @param modules The swerve modules.
     * @return A new {@link SwerveDriveOdometry} instance.
     */
    public static SwerveDriveOdometry createOdometry(Supplier<Angle> gyro, Pose2d initialPose, SwerveModule... modules) {
        return new SwerveDriveOdometry(
            createKinematics(modules),
            Rotation2d.fromRadians(gyro.get().in(Radians)),
            getModulePositions(modules),
            initialPose
        );
    }

    /**
     * Creates a new pose estimator for sensor fusion using vision and odometry.
     *
     * @param gyro The gyro supplier.
     * @param initialPose The initial pose to start from.
     * @param modules The swerve modules.
     * @return A new {@link SwerveDrivePoseEstimator} instance.
     */
    public static SwerveDrivePoseEstimator createPoseEstimator(Supplier<Angle> gyro, Pose2d initialPose, SwerveModule... modules) {
        return new SwerveDrivePoseEstimator(
            createKinematics(modules),
            Rotation2d.fromRadians(gyro.get().in(Radians)),
            SwerveDrive.getModulePositions(modules),
            initialPose
        );
    }

    /**
     * Creates a new pose estimator with custom state and vision standard deviations.
     *
     * @param gyro The gyro supplier.
     * @param initialPose The initial pose to start from.
     * @param stateStdDevs The standard deviations of the model states.
     * @param visionStdDevs The standard deviations of the vision measurements.
     * @param modules The swerve modules.
     * @return A new {@link SwerveDrivePoseEstimator} instance.
     */
    public static SwerveDrivePoseEstimator createPoseEstimator(
        Supplier<Angle> gyro,
        Pose2d initialPose,
        Matrix<N3,N1> stateStdDevs,
        Matrix<N3,N1> visionStdDevs,
        SwerveModule... modules
    ) {
        return new SwerveDrivePoseEstimator(
            createKinematics(modules),
            Rotation2d.fromRadians(gyro.get().in(Radians)),
            SwerveDrive.getModulePositions(modules),
            initialPose,
            stateStdDevs,
            visionStdDevs
        );
    }
}
