// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.swervebotnew.subsystems;

import static edu.wpi.first.units.Units.Degrees;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Rotations;


import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.TrapezoidProfile.Constraints;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;
import edu.wpi.first.wpilibj.DutyCycleEncoder;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.examples.swervebotnew.Constants.DriveConstants;
import edu.wpi.first.wpilibj.examples.swervebotnew.Constants.ModuleConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj.drive.SwerveDrive;
import edu.wpi.first.wpilibj.drive.SwerveModule;

public class DriveSubsystem extends SubsystemBase {

    private final SwerveDrive m_swerveDrive;
    private final SwerveModule m_frontLeftModule = createSwerveModule(
        DriveConstants.kFrontLeftModuleLocation,
        DriveConstants.kFrontLeftDriveMotorPort,
        DriveConstants.kFrontLeftTurningMotorPort,
        DriveConstants.kFrontLeftDriveEncoderPorts,
        DriveConstants.kFrontLeftTurningEncoderPorts
    );
    
    private final SwerveModule m_rearLeftModule = createSwerveModule(
        DriveConstants.kRearLeftModuleLocation,
        DriveConstants.kRearLeftDriveMotorPort,
        DriveConstants.kRearLeftTurningMotorPort,
        DriveConstants.kRearLeftDriveEncoderPorts,
        DriveConstants.kRearLeftTurningEncoderPorts
    );
    
    private final SwerveModule m_frontRightModule = createSwerveModule(
        DriveConstants.kFrontRightModuleLocation,
        DriveConstants.kFrontRightDriveMotorPort,
        DriveConstants.kFrontRightTurningMotorPort,
        DriveConstants.kFrontRightDriveEncoderPorts,
        DriveConstants.kFrontRightTurningEncoderPorts
    );
    
    private final SwerveModule m_rearRightModule = createSwerveModule(
        DriveConstants.kRearRightModuleLocation,
        DriveConstants.kRearRightDriveMotorPort,
        DriveConstants.kRearRightTurningMotorPort,
        DriveConstants.kRearRightDriveEncoderPorts,
        DriveConstants.kRearRightTurningEncoderPorts
    );

    private final ADXRS450_Gyro m_gyro = new ADXRS450_Gyro();

    public DriveSubsystem() {
        // Create the SwerveDrive object with the modules and gyro supplier
        m_swerveDrive = new SwerveDrive(
            this::getGyroAngle,  // Pass the gyro angle using a method reference
            m_frontLeftModule,
            m_rearLeftModule,
            m_frontRightModule,
            m_rearRightModule
        );
    }

    // Helper function to create a swerve module
    private SwerveModule createSwerveModule(Translation2d location, int driveMotorPort, int turningMotorPort,
                                            int[] driveEncoderPorts, int[] turningEncoderPorts) {

        PWMSparkMax driveMotor = new PWMSparkMax(driveMotorPort);
        PWMSparkMax turningMotor = new PWMSparkMax(turningMotorPort);

        // Create encoders for both drive and turning motors
        Encoder driveEncoder = new Encoder(driveEncoderPorts[0], driveEncoderPorts[1]);
        DutyCycleEncoder turningEncoder = new DutyCycleEncoder(turningEncoderPorts[0]);

        // Encoder settings
        driveEncoder.setDistancePerPulse(ModuleConstants.kDriveEncoderDistancePerPulse);

        // Create PID controllers for the drive motor
        PIDController drivePIDController = new PIDController(
            ModuleConstants.kPModuleDriveController,
            ModuleConstants.kIModuleDriveController,
            ModuleConstants.kDModuleDriveController
        );

        // Create the PID controller for the turning motor
        ProfiledPIDController turningPidController = new ProfiledPIDController(
            ModuleConstants.kPModuleTurningController,
            ModuleConstants.kIModuleTurningController,
            ModuleConstants.kDModuleTurningController,
            new Constraints(
                ModuleConstants.kMaxModuleAngularSpeedRadiansPerSecond,
                ModuleConstants.kMaxModuleAngularAccelerationRadiansPerSecondSquared
            )
        );

        // Create the SimpleMotorFeedforward for the turning motor
        SimpleMotorFeedforward turningFeedforward = new SimpleMotorFeedforward(
            ModuleConstants.ksVoltsTurning,  // Static gain
            ModuleConstants.kvVoltsPerSecondTurning   // Velocity gain
        );

        // Simple motor feedforward models for drive motor
        SimpleMotorFeedforward driveFeedforward = new SimpleMotorFeedforward(
            DriveConstants.ksVolts,
            DriveConstants.kvVoltSecondsPerMeter,
            DriveConstants.kaVoltSecondsSquaredPerMeter
        );

        // Create the SwerveModule object
        return new SwerveModule(
            location,
            voltage -> driveMotor.setVoltage(voltage),
            voltage -> turningMotor.setVoltage(voltage),
            turningPidController,
            () -> Rotations.of(turningEncoder.get()),
            turningFeedforward,
            drivePIDController,
            () -> MetersPerSecond.of(driveEncoder.getRate()),
            () -> Meters.of(driveEncoder.getDistance()),
            driveFeedforward
        );
    }

    // Method to drive the swerve drive system using the SwerveModuleStates
    public void drive(double forwardSpeed, double strafeSpeed, double rotationSpeed) {
        // Create ChassisSpeeds from forward, strafe, and rotation speeds
        ChassisSpeeds chassisSpeeds = new ChassisSpeeds(forwardSpeed, strafeSpeed, rotationSpeed);

        // Apply the chassis speeds to the swerve drive system
        m_swerveDrive.applyChassisSpeeds(chassisSpeeds);
    }

    // Resets the pose of the robot
    public void resetPose(Pose2d newPose) {
        m_swerveDrive.resetPose(newPose);
    }

    // Updates the odometry
    @Override
    public void periodic() {
        // Update the odometry every time the periodic function is called
        m_swerveDrive.updateOdometry();
    }

    // Get the current estimated pose of the robot
    public Pose2d getPose() {
        return m_swerveDrive.getPoseMeters();  // Return the robot's pose from odometry
    }

    // Get the current gyro angle
    public Angle getGyroAngle() {
        return Degrees.of(m_gyro.getAngle());
    }

    // Reset the gyro (if needed)
    public void resetGyro() {
        m_gyro.reset();
    }
}
