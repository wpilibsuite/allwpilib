// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.eventloop;

import org.wpilib.driverstation.Joystick;
import org.wpilib.event.BooleanEvent;
import org.wpilib.event.EventLoop;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;

public class Robot extends TimedRobot {
  public static final double SHOT_VELOCITY = 200; // rpm
  public static final double TOLERANCE = 8; // rpm

  private final PWMSparkMax shooter = new PWMSparkMax(0);
  private final Encoder shooterEncoder = new Encoder(0, 1);
  private final PIDController controller = new PIDController(0.3, 0, 0);
  private final SimpleMotorFeedforward ff = new SimpleMotorFeedforward(0.1, 0.065);

  private final PWMSparkMax kicker = new PWMSparkMax(1);

  private final PWMSparkMax intake = new PWMSparkMax(2);

  private final EventLoop loop = new EventLoop();
  private final Joystick joystick = new Joystick(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    controller.setTolerance(TOLERANCE);

    BooleanEvent isBallAtKicker =
        new BooleanEvent(loop, () -> false); // kickerSensor.getRange() < KICKER_THRESHOLD);
    BooleanEvent intakeButton = new BooleanEvent(loop, () -> joystick.getRawButton(2));

    // if the thumb button is held
    intakeButton
        // and there is not a ball at the kicker
        .and(isBallAtKicker.negate())
        // activate the intake
        .ifHigh(() -> intake.setThrottle(0.5));

    // if the thumb button is not held
    intakeButton
        .negate()
        // or there is a ball in the kicker
        .or(isBallAtKicker)
        // stop the intake
        .ifHigh(intake::stopMotor);

    BooleanEvent shootTrigger = new BooleanEvent(loop, joystick::getTrigger);

    // if the trigger is held
    shootTrigger
        // accelerate the shooter wheel
        .ifHigh(
        () -> {
          shooter.setVoltage(
              controller.calculate(shooterEncoder.getRate(), SHOT_VELOCITY)
                  + ff.calculate(SHOT_VELOCITY));
        });

    // if not, stop
    shootTrigger.negate().ifHigh(shooter::stopMotor);

    BooleanEvent atTargetVelocity =
        new BooleanEvent(loop, controller::atSetpoint)
            // debounce for more stability
            .debounce(0.2);

    // if we're at the target velocity, kick the ball into the shooter wheel
    atTargetVelocity.ifHigh(() -> kicker.setThrottle(0.7));

    // when we stop being at the target velocity, it means the ball was shot
    atTargetVelocity
        .falling()
        // so stop the kicker
        .ifHigh(kicker::stopMotor);
  }

  @Override
  public void robotPeriodic() {
    // poll all the bindings
    loop.poll();
  }
}
