// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.eventloop;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.Ultrasonic;
import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Robot extends TimedRobot {
  public static final int SHOT_VELOCITY = 200; // rpm
  public static final int TOLERANCE = 8; // rpm
  public static final int KICKER_THRESHOLD = 15; // mm

  private final PWMSparkMax m_shooter = new PWMSparkMax(0);
  private final Encoder m_shooterEncoder = new Encoder(0, 1);
  private final PIDController m_controller = new PIDController(0.3, 0, 0);
  private final SimpleMotorFeedforward m_ff = new SimpleMotorFeedforward(0.1, 0.065);

  private final PWMSparkMax m_kicker = new PWMSparkMax(1);
  private final Ultrasonic m_kickerSensor = new Ultrasonic(2, 3);

  private final PWMSparkMax m_intake = new PWMSparkMax(2);

  private final EventLoop m_loop = new EventLoop();
  private final Joystick m_joystick = new Joystick(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    m_controller.setTolerance(TOLERANCE);

    BooleanEvent isBallAtKicker =
        new BooleanEvent(m_loop, () -> m_kickerSensor.getRangeMM() < KICKER_THRESHOLD);
    BooleanEvent intakeButton = new BooleanEvent(m_loop, () -> m_joystick.getRawButton(2));

    // if the thumb button is held
    intakeButton
        // and there is not a ball at the kicker
        .and(isBallAtKicker.negate())
        // activate the intake
        .ifHigh(() -> m_intake.set(0.5));

    // if the thumb button is not held
    intakeButton
        .negate()
        // or there is a ball in the kicker
        .or(isBallAtKicker)
        // stop the intake
        .ifHigh(m_intake::stopMotor);

    BooleanEvent shootTrigger = new BooleanEvent(m_loop, m_joystick::getTrigger);

    // if the trigger is held
    shootTrigger
        // accelerate the shooter wheel
        .ifHigh(
        () -> {
          m_shooter.setVoltage(
              m_controller.calculate(m_shooterEncoder.getRate(), SHOT_VELOCITY)
                  + m_ff.calculate(SHOT_VELOCITY));
        });

    // if not, stop
    shootTrigger.negate().ifHigh(m_shooter::stopMotor);

    BooleanEvent atTargetVelocity =
        new BooleanEvent(m_loop, m_controller::atSetpoint)
            // debounce for more stability
            .debounce(0.2);

    // if we're at the target velocity, kick the ball into the shooter wheel
    atTargetVelocity.ifHigh(() -> m_kicker.set(0.7));

    // when we stop being at the target velocity, it means the ball was shot
    atTargetVelocity
        .falling()
        // so stop the kicker
        .ifHigh(m_kicker::stopMotor);
  }

  @Override
  public void robotPeriodic() {
    // poll all the bindings
    m_loop.poll();
  }
}
