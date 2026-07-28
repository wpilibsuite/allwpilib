// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.Degrees;
import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.RPM;
import static org.wpilib.units.Units.Radians;

import java.util.function.Supplier;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.ShooterConstants;
import org.wpilib.examples.rebuiltcmdv3.lookup.LookupTables;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.math.interpolation.InterpolatingTreeMap;
import org.wpilib.units.measure.Angle;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.Distance;

@Logged
public class Shooter implements Mechanism {
  private static final InterpolatingTreeMap<Distance, Angle> HOOD_LUT =
      new InterpolatingTreeMap<>(
          LookupTables.inverseUnitInterpolator(), LookupTables.unitInterpolator());
  private static final InterpolatingTreeMap<Distance, AngularVelocity> FLYWHEEL_LUT =
      new InterpolatingTreeMap<>(
          LookupTables.inverseUnitInterpolator(), LookupTables.unitInterpolator());

  static {
    // Populate the lookup tables
    HOOD_LUT.put(Meters.of(0), Degrees.of(0));
    HOOD_LUT.put(Meters.of(1), Degrees.of(5));
    HOOD_LUT.put(Meters.of(2), Degrees.of(10));
    HOOD_LUT.put(Meters.of(3), Degrees.of(20));
    HOOD_LUT.put(Meters.of(4), Degrees.of(30));

    FLYWHEEL_LUT.put(Meters.of(0), RPM.of(1000));
    FLYWHEEL_LUT.put(Meters.of(1), RPM.of(1000));
    FLYWHEEL_LUT.put(Meters.of(2), RPM.of(2000));
    FLYWHEEL_LUT.put(Meters.of(3), RPM.of(3000));
    FLYWHEEL_LUT.put(Meters.of(4), RPM.of(4000));
  }

  private final ExampleSmartMotorController hoodMotor;

  private final ExampleSmartMotorController flywheelMotorPrimary;
  private final ExampleSmartMotorController flywheelMotorSecondary;
  private final ExampleSmartMotorController flywheelMotorTertiary;
  private final ExampleSmartMotorController flywheelMotorQuatenary;

  /**
   * Creates a new shooter mechanism object and initializes its motors. Only one shooter mechanism
   * should exist at a time.
   */
  public Shooter() {
    hoodMotor = new ExampleSmartMotorController(ShooterConstants.HOOD_MOTOR_ID);
    flywheelMotorPrimary =
        new ExampleSmartMotorController(ShooterConstants.PRIMARY_SHOOTER_MOTOR_ID);
    flywheelMotorSecondary =
        new ExampleSmartMotorController(ShooterConstants.SECONDARY_SHOOTER_MOTOR_ID);
    flywheelMotorTertiary =
        new ExampleSmartMotorController(ShooterConstants.TERTIARY_SHOOTER_MOTOR_ID);
    flywheelMotorQuatenary =
        new ExampleSmartMotorController(ShooterConstants.QUATERNARY_SHOOTER_MOTOR_ID);

    flywheelMotorSecondary.follow(flywheelMotorPrimary);
    flywheelMotorTertiary.follow(flywheelMotorPrimary);
    flywheelMotorQuatenary.follow(flywheelMotorPrimary);

    hoodMotor.setPID(ShooterConstants.HOOD_KP, 0, 0);
    flywheelMotorPrimary.setPID(ShooterConstants.FLYWHEEL_KP, 0, 0);

    setDefaultCommand(idle());
  }

  @Override
  public Command idle() {
    return run(coroutine -> {
          coroutine.awaitAll(runHoodAngle(Degrees::zero), runFlywheelSpeed(RPM::zero));
        })
        .withPriority(Command.LOWEST_PRIORITY)
        .named("Shooter.Idle");
  }

  /**
   * Shoots at the hub. The flywheel and hood are automatically adjusted based on the distance to
   * the center of the hub. This could potentially be used for passing as well; but, because the
   * center of the hub is six feet above the ground, the distance would need to be fudged to account
   * for the height difference.
   *
   * @param targetDistance The distance to the target, measured from the center of the robot to the
   *     center of the target.
   * @return A command that shoots at the hub
   */
  public Command shootAtHub(Supplier<Distance> targetDistance) {
    // Because this command is used when only the distance is known,
    // we need to do a little bit of work to convert from a target distance to
    // individual hood angle and flywheel speed values.
    Supplier<Angle> hoodAngle = () -> HOOD_LUT.get(targetDistance.get());
    Supplier<AngularVelocity> flywheelSpeed = () -> FLYWHEEL_LUT.get(targetDistance.get());

    return run(coroutine -> {
          coroutine.awaitAll(runHoodAngle(hoodAngle), runFlywheelSpeed(flywheelSpeed));
        })
        .named("Shooter.ShootAtHub");
  }

  // Private commands to be used by compositions

  /**
   * Moves the hood to the specified angle and holds it. This command will run forever unless
   * interrupted.
   *
   * @param angle A dynamic supplier for the desired hood angle.
   * @return The hood angle command
   */
  private Command runHoodAngle(Supplier<Angle> angle) {
    return runRepeatedly(
            () -> {
              hoodMotor.setSetpoint(
                  ExampleSmartMotorController.PIDMode.kPosition, angle.get().in(Radians));
            })
        .named("Shooter.RunHoodAngle");
  }

  /**
   * Runs the flywheel at the specified speed and holds it. This command will run forever unless
   * interrupted.
   *
   * @param speed A dynamic supplier for the desired flywheel speed.
   * @return The flywheel speed command
   */
  private Command runFlywheelSpeed(Supplier<AngularVelocity> speed) {
    return runRepeatedly(
            () -> {
              flywheelMotorPrimary.setSetpoint(
                  ExampleSmartMotorController.PIDMode.kVelocity, speed.get().in(RPM));
            })
        .named("Shooter.RunFlywheelSpeed");
  }
}
