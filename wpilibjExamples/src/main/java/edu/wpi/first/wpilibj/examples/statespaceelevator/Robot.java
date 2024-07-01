// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.statespaceelevator;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.LinearQuadraticRegulator;
import edu.wpi.first.math.estimator.KalmanFilter;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.LinearSystemLoop;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program to demonstrate how to use a state-space controller to control an
 * elevator.
 */
public class Robot extends TimedRobot {
  private static final int kMotorPort = 0;
  private static final int kEncoderAChannel = 0;
  private static final int kEncoderBChannel = 1;
  private static final int kJoystickPort = 0;
  private static final double kHighGoalPosition = Units.feetToMeters(3);
  private static final double kLowGoalPosition = Units.feetToMeters(0);

  private static final double kCarriageMass = 4.5; // kilograms

  // A 1.5in diameter drum has a radius of 0.75in, or 0.019in.
  private static final double kDrumRadius = 1.5 / 2.0 * 25.4 / 1000.0;

  // Reduction between motors and encoder, as output over input. If the elevator spins slower than
  // the motors, this number should be greater than one.
  private static final double kElevatorGearing = 6.0;

  private final TrapezoidProfile m_profile =
      new TrapezoidProfile(
          new TrapezoidProfile.Constraints(
              Units.feetToMeters(3.0),
              Units.feetToMeters(6.0))); // Max elevator speed and acceleration.
  private TrapezoidProfile.State m_lastProfiledReference = new TrapezoidProfile.State();

  /* The plant holds a state-space model of our elevator. This system has the following properties:

  States: [position, velocity], in meters and meters per second.
  Inputs (what we can "put in"): [voltage], in volts.
  Outputs (what we can measure): [position], in meters.

  This elevator is driven by two NEO motors.
   */
  private final LinearSystem<N2, N1, N2> m_elevatorPlant =
      LinearSystemId.createElevatorSystem(
          DCMotor.getNEO(2), kCarriageMass, kDrumRadius, kElevatorGearing);

  // The observer fuses our encoder data and voltage inputs to reject noise.
  @SuppressWarnings("unchecked")
  private final KalmanFilter<N2, N1, N1> m_observer =
      new KalmanFilter<>(
          Nat.N2(),
          Nat.N1(),
          (LinearSystem<N2, N1, N1>) m_elevatorPlant.slice(0),
          VecBuilder.fill(Units.inchesToMeters(2), Units.inchesToMeters(40)), // How accurate we
          // think our model is, in meters and meters/second.
          VecBuilder.fill(0.001), // How accurate we think our encoder position
          // data is. In this case we very highly trust our encoder position reading.
          0.020);

  // A LQR uses feedback to create voltage commands.
  @SuppressWarnings("unchecked")
  private final LinearQuadraticRegulator<N2, N1, N1> m_controller =
      new LinearQuadraticRegulator<>(
          (LinearSystem<N2, N1, N1>) m_elevatorPlant.slice(0),
          VecBuilder.fill(Units.inchesToMeters(1.0), Units.inchesToMeters(10.0)), // qelms. Position
          // and velocity error tolerances, in meters and meters per second. Decrease this to more
          // heavily penalize state excursion, or make the controller behave more aggressively. In
          // this example we weight position much more highly than velocity, but this can be
          // tuned to balance the two.
          VecBuilder.fill(12.0), // relms. Control effort (voltage) tolerance. Decrease this to more
          // heavily penalize control effort, or make the controller less aggressive. 12 is a good
          // starting point because that is the (approximate) maximum voltage of a battery.
          0.020); // Nominal time between loops. 0.020 for TimedRobot, but can be

  // lower if using notifiers.

  // The state-space loop combines a controller, observer, feedforward and plant for easy control.
  @SuppressWarnings("unchecked")
  private final LinearSystemLoop<N2, N1, N1> m_loop =
      new LinearSystemLoop<>(
          (LinearSystem<N2, N1, N1>) m_elevatorPlant.slice(0),
          m_controller,
          m_observer,
          12.0,
          0.020);

  // An encoder set up to measure elevator height in meters.
  private final Encoder m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);

  private final PWMSparkMax m_motor = new PWMSparkMax(kMotorPort);

  // A joystick to read the trigger from.
  private final Joystick m_joystick = new Joystick(kJoystickPort);

  public Robot() {
    // Circumference = pi * d, so distance per click = pi * d / counts
    m_encoder.setDistancePerPulse(Math.PI * 2 * kDrumRadius / 4096.0);
  }

  @Override
  public void teleopInit() {
    // Reset our loop to make sure it's in a known state.
    m_loop.reset(VecBuilder.fill(m_encoder.getDistance(), m_encoder.getRate()));

    // Reset our last reference to the current state.
    m_lastProfiledReference =
        new TrapezoidProfile.State(m_encoder.getDistance(), m_encoder.getRate());
  }

  @Override
  public void teleopPeriodic() {
    // Sets the target position of our arm. This is similar to setting the setpoint of a
    // PID controller.
    TrapezoidProfile.State goal;
    if (m_joystick.getTrigger()) {
      // the trigger is pressed, so we go to the high goal.
      goal = new TrapezoidProfile.State(kHighGoalPosition, 0.0);
    } else {
      // Otherwise, we go to the low goal
      goal = new TrapezoidProfile.State(kLowGoalPosition, 0.0);
    }
    // Step our TrapezoidalProfile forward 20ms and set it as our next reference
    m_lastProfiledReference = m_profile.calculate(0.020, m_lastProfiledReference, goal);
    m_loop.setNextR(m_lastProfiledReference.position, m_lastProfiledReference.velocity);

    // Correct our Kalman filter's state vector estimate with encoder data.
    m_loop.correct(VecBuilder.fill(m_encoder.getDistance()));

    // Update our LQR to generate new voltage commands and use the voltages to predict the next
    // state with out Kalman filter.
    m_loop.predict(0.020);

    // Send the new calculated voltage to the motors.
    // voltage = duty cycle * battery voltage, so
    // duty cycle = voltage / battery voltage
    double nextVoltage = m_loop.getU(0);
    m_motor.setVoltage(nextVoltage);
  }
}
