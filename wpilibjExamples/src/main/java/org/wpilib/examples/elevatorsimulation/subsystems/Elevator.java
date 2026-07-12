// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorsimulation.subsystems;

import org.wpilib.examples.elevatorsimulation.Constants;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.ProfiledPIDController;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.ElevatorSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.Mechanism2d;
import org.wpilib.smartdashboard.MechanismLigament2d;
import org.wpilib.smartdashboard.MechanismRoot2d;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;

public class Elevator implements AutoCloseable {
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  private final DCMotor elevatorGearbox = DCMotor.getVex775Pro(4);

  // Standard classes for controlling our elevator
  private final ProfiledPIDController controller =
      new ProfiledPIDController(
          Constants.kElevatorKp,
          Constants.kElevatorKi,
          Constants.kElevatorKd,
          new TrapezoidProfile.Constraints(2.45, 2.45));
  ElevatorFeedforward feedforward =
      new ElevatorFeedforward(
          Constants.kElevatorkS,
          Constants.kElevatorkG,
          Constants.kElevatorkV,
          Constants.kElevatorkA);
  private final Encoder encoder =
      new Encoder(Constants.kEncoderAChannel, Constants.kEncoderBChannel);
  private final PWMSparkMax motor = new PWMSparkMax(Constants.kMotorPort);

  // Simulation classes help us simulate what's going on, including gravity.
  private final ElevatorSim elevatorSim =
      new ElevatorSim(
          elevatorGearbox,
          Constants.kElevatorGearing,
          Constants.kCarriageMass,
          Constants.kElevatorDrumRadius,
          Constants.kMinElevatorHeight,
          Constants.kMaxElevatorHeight,
          true,
          0,
          0.01,
          0.0);
  private final EncoderSim encoderSim = new EncoderSim(encoder);
  private final PWMMotorControllerSim motorSim = new PWMMotorControllerSim(motor);

  // Create a Mechanism2d visualization of the elevator
  private final Mechanism2d mech2d = new Mechanism2d(20, 50);
  private final MechanismRoot2d mech2dRoot = mech2d.getRoot("Elevator Root", 10, 0);
  private final MechanismLigament2d elevatorMech2d =
      mech2dRoot.append(new MechanismLigament2d("Elevator", elevatorSim.getPosition(), 90));

  /** Subsystem constructor. */
  public Elevator() {
    encoder.setDistancePerPulse(Constants.kElevatorEncoderDistPerPulse);

    // Publish Mechanism2d to SmartDashboard
    // To view the Elevator visualization, select Network Tables -> SmartDashboard -> Elevator Sim
    SmartDashboard.putData("Elevator Sim", mech2d);
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
    controller.setGoal(goal);

    // With the setpoint value we run PID control like normal
    double pidOutput = controller.calculate(encoder.getDistance());
    double feedforwardOutput = feedforward.calculate(controller.getSetpoint().velocity);
    motor.setVoltage(pidOutput + feedforwardOutput);
  }

  /** Stop the control loop and motor output. */
  public void stop() {
    controller.setGoal(0.0);
    motor.setThrottle(0.0);
  }

  /** Update telemetry, including the mechanism visualization. */
  public void updateTelemetry() {
    // Update elevator visualization with position
    elevatorMech2d.setLength(encoder.getDistance());
  }

  @Override
  public void close() {
    encoder.close();
    motor.close();
    mech2d.close();
  }
}
