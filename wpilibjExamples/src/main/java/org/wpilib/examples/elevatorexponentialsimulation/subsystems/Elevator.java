// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorexponentialsimulation.subsystems;

import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.examples.elevatorexponentialsimulation.Constants;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.trajectory.ExponentialProfile;
import org.wpilib.math.util.Units;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.ElevatorSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.Mechanism2d;
import org.wpilib.smartdashboard.MechanismLigament2d;
import org.wpilib.smartdashboard.MechanismRoot2d;
import org.wpilib.system.RobotController;
import org.wpilib.telemetry.Telemetry;

public class Elevator implements AutoCloseable {
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  private final DCMotor elevatorGearbox = DCMotor.getNEO(2);

  private final ExponentialProfile profile =
      new ExponentialProfile(
          ExponentialProfile.Constraints.fromCharacteristics(
              Constants.ELEVATOR_MAX_V, Constants.ELEVATOR_KV, Constants.ELEVATOR_KA));

  private ExponentialProfile.State setpoint = new ExponentialProfile.State(0, 0);

  // Standard classes for controlling our elevator
  private final PIDController pidController =
      new PIDController(Constants.ELEVATOR_KP, Constants.ELEVATOR_KI, Constants.ELEVATOR_KD);

  ElevatorFeedforward feedforward =
      new ElevatorFeedforward(
          Constants.ELEVATOR_KS,
          Constants.ELEVATOR_KG,
          Constants.ELEVATOR_KV,
          Constants.ELEVATOR_KA);
  private final Encoder encoder =
      new Encoder(Constants.ENCODER_A_CHANNEL, Constants.ENCODER_B_CHANNEL);
  private final PWMSparkMax motor = new PWMSparkMax(Constants.MOTOR_PORT);

  // Simulation classes help us simulate what's going on, including gravity.
  private final ElevatorSim elevatorSim =
      new ElevatorSim(
          elevatorGearbox,
          Constants.ELEVATOR_GEARING,
          Constants.CARRIAGE_MASS,
          Constants.ELEVATOR_DRUM_RADIUS,
          Constants.MIN_ELEVATOR_HEIGHT,
          Constants.MAX_ELEVATOR_HEIGHT,
          true,
          0,
          0.005,
          0.0);
  private final EncoderSim encoderSim = new EncoderSim(encoder);
  private final PWMMotorControllerSim motorSim = new PWMMotorControllerSim(motor);

  // Create a Mechanism2d visualization of the elevator
  private final Mechanism2d mech2d =
      new Mechanism2d(Units.inchesToMeters(10), Units.inchesToMeters(51));
  private final MechanismRoot2d mech2dRoot =
      mech2d.getRoot("Elevator Root", Units.inchesToMeters(5), Units.inchesToMeters(0.5));
  private final MechanismLigament2d elevatorMech2d =
      mech2dRoot.append(new MechanismLigament2d("Elevator", elevatorSim.getPosition(), 90));

  /** Subsystem constructor. */
  public Elevator() {
    encoder.setDistancePerPulse(Constants.ELEVATOR_ENCODER_DIST_PER_PULSE);
  }

  /** Advance the simulation. */
  public void simulationPeriodic() {
    // In this method, we update our simulation of what our elevator is doing
    // First, we set our "inputs" (voltages)
    elevatorSim.setInput(motorSim.getThrottle() * RobotController.getBatteryVoltage());

    // Next, we update it. The standard loop time is 20ms.
    elevatorSim.update(0.020);

    // Finally, we set our simulated encoder's readings and simulated battery voltage
    encoderSim.setDistance(elevatorSim.getPosition());
    // SimBattery estimates loaded battery voltages
    RoboRioSim.setVInVoltage(
        BatterySim.calculateDefaultBatteryLoadedVoltage(elevatorSim.getCurrentDraw()));
  }

  /**
   * Run control loop to reach and maintain goal.
   *
   * @param goal the position to maintain
   */
  public void reachGoal(double goal) {
    var goalState = new ExponentialProfile.State(goal, 0);

    var next = profile.calculate(0.020, setpoint, goalState);

    // With the setpoint value we run PID control like normal
    double pidOutput = pidController.calculate(encoder.getDistance(), setpoint.position);
    double feedforwardOutput = feedforward.calculate(setpoint.velocity, next.velocity);

    motor.setVoltage(pidOutput + feedforwardOutput);

    setpoint = next;
  }

  /** Stop the control loop and motor output. */
  public void stop() {
    motor.setThrottle(0.0);
  }

  /** Reset Exponential profile to begin from current position on enable. */
  public void reset() {
    setpoint = new ExponentialProfile.State(encoder.getDistance(), 0);
  }

  /** Update telemetry, including the mechanism visualization. */
  public void updateTelemetry() {
    // Update elevator visualization with position
    elevatorMech2d.setLength(encoder.getDistance());

    // Publish Mechanism2d to telemetry
    // To view the Elevator visualization, select Network Tables -> Telemetry -> Elevator Sim
    Telemetry.log("Elevator Sim", mech2d);
  }

  @Override
  public void close() {
    encoder.close();
    motor.close();
    mech2d.close();
  }
}
