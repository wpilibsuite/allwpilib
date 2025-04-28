// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import java.util.function.Consumer;
import java.util.function.Supplier;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.ProfiledPIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Voltage;


import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.Rotations;
import static edu.wpi.first.units.Units.Volts;
/**
 * Represents a single swerve module, which includes a drive and steering (azimuth) motor.
 * <p>
 * Handles PID control, feedforward, and optimization logic for an individual module
 * in a swerve drive system.
 */
public class SwerveModule {
    /** Function to apply voltage to the drive motor. */
    private final Consumer<Voltage> m_driveVoltageConsumer;

    /** Function to apply voltage to the steering (azimuth) motor. */
    private final Consumer<Voltage> m_steeringVoltageConsumer;

    /** Supplier for the current steering angle of the module. */
    private final Supplier<Angle> m_steeringAngleSupplier;

    /** PID controller for the steering motor. */
    private final ProfiledPIDController m_steeringPIDController;

    /** Supplier for the current velocity of the drive motor. */
    private final Supplier<LinearVelocity> m_driveVelocitySupplier;

    /** Supplier for the total distance traveled by the drive motor. */
    private final Supplier<Distance> m_driveDistanceSupplier;

    /** PID controller for the drive motor. */
    private final PIDController m_drivePIDController;

    /** The physical location of this module on the robot's chassis. */
    private final Translation2d m_modulePosition;

    /** Feedforward model for the drive motor. */
    private final SimpleMotorFeedforward m_driveFeedforwardModel;

    /** Feedforward model for the steering motor. */
    private final SimpleMotorFeedforward m_steeringFeedforwardModel;

    /**
     * Constructs a new {@code SwerveModule}.
     *
     * @param modulePosition The location of this module on the robot frame (used for kinematics).
     * @param driveVoltageConsumer Function to apply voltage to the drive motor.
     * @param steeringVoltageConsumer Function to apply voltage to the steering motor.
     * @param steeringPIDController PID controller for the steering motor.
     * @param steeringAngleSupplier Supplier that returns the current steering angle.
     * @param steeringFeedforwardModel Feedforward model for the steering motor.
     * @param drivePIDController PID controller for the drive motor.
     * @param driveVelocitySupplier Supplier that returns the current drive speed.
     * @param driveDistanceSupplier Supplier that returns the total driven distance.
     * @param driveFeedforwardModel Feedforward model for the drive motor.
     */
    public SwerveModule(
        Translation2d modulePosition,
        Consumer<Voltage> driveVoltageConsumer,
        Consumer<Voltage> steeringVoltageConsumer,
        ProfiledPIDController steeringPIDController,
        Supplier<Angle> steeringAngleSupplier,
        SimpleMotorFeedforward steeringFeedforwardModel,
        PIDController drivePIDController,
        Supplier<LinearVelocity> driveVelocitySupplier,
        Supplier<Distance> driveDistanceSupplier,
        SimpleMotorFeedforward driveFeedforwardModel
    ) {
        this.m_modulePosition = modulePosition;
        this.m_driveVoltageConsumer = driveVoltageConsumer;
        this.m_steeringVoltageConsumer = steeringVoltageConsumer;
        this.m_steeringAngleSupplier = steeringAngleSupplier;
        this.m_steeringPIDController = steeringPIDController;
        this.m_driveVelocitySupplier = driveVelocitySupplier;
        this.m_drivePIDController = drivePIDController;
        this.m_driveDistanceSupplier = driveDistanceSupplier;
        this.m_steeringFeedforwardModel = steeringFeedforwardModel;
        this.m_driveFeedforwardModel = driveFeedforwardModel;

        steeringPIDController.enableContinuousInput(-Math.PI, Math.PI);
    }

    /**
     * Applies a desired {@link SwerveModuleState} to the module.
     *
     * @param state The desired swerve module state.
     */
    public void applyState(SwerveModuleState state) {
        Rotation2d currentAngle = Rotation2d.fromRadians(m_steeringAngleSupplier.get().in(Radians));

        state.optimize(currentAngle);
        state.cosineScale(currentAngle);

        double driveOutput = m_drivePIDController.calculate(
            m_driveVelocitySupplier.get().in(MetersPerSecond),
            state.speedMetersPerSecond
        ) + m_driveFeedforwardModel.calculate(m_driveVelocitySupplier.get().in(MetersPerSecond));

        double steeringOutput = m_steeringPIDController.calculate(
            m_steeringAngleSupplier.get().in(Radians),
            state.angle.getRadians()
        ) + m_steeringFeedforwardModel.calculate(m_steeringPIDController.getSetpoint().velocity);

        m_driveVoltageConsumer.accept(Volts.of(driveOutput));
        m_steeringVoltageConsumer.accept(Volts.of(steeringOutput));
    }

    /**
     * Returns the current state of the swerve module.
     *
     * @return The current {@link SwerveModuleState}.
     */
    public SwerveModuleState getState() {
        return new SwerveModuleState(
            m_driveVelocitySupplier.get(),
            Rotation2d.fromRotations(m_steeringAngleSupplier.get().in(Rotations))
        );
    }

    /**
     * Returns the encoder-based position of the swerve module.
     *
     * @return The {@link SwerveModulePosition}, including wheel travel and steering angle.
     */
    public SwerveModulePosition getPosition() {
        return new SwerveModulePosition(
            m_driveDistanceSupplier.get(),
            Rotation2d.fromRotations(m_steeringAngleSupplier.get().in(Rotations))
        );
    }

    /**
     * Returns the fixed location of the module on the robot chassis.
     *
     * @return The module's {@link Translation2d} position.
     */
    public Translation2d getModuleLocation() {
        return m_modulePosition;
    }
}
