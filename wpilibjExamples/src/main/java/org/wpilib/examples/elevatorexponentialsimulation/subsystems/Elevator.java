// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorexponentialsimulation.subsystems;

import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.system.plant.DCMotor;
import org.wpilib.math.trajectory.ExponentialProfile;
import org.wpilib.math.util.Units;
import org.wpilib.Encoder;
import org.wpilib.RobotController;
import org.wpilib.examples.elevatorexponentialsimulation.Constants;
import org.wpilib.motorcontrol.PWMSparkMax;
import org.wpilib.simulation.BatterySim;
import org.wpilib.simulation.ElevatorSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.smartdashboard.Mechanism2d;
import org.wpilib.smartdashboard.MechanismLigament2d;
import org.wpilib.smartdashboard.MechanismRoot2d;
import org.wpilib.smartdashboard.SmartDashboard;

public class Elevator implements AutoCloseable {
  // This gearbox represents a gearbox containing 4 Vex 775pro motors.
  private final DCMotor m_elevatorGearbox = DCMotor.getNEO(2);

  private final ExponentialProfile m_profile =
      new ExponentialProfile(
          ExponentialProfile.Constraints.fromCharacteristics(
              Constants.kElevatorMaxV, Constants.kElevatorkV, Constants.kElevatorkA));

  private ExponentialProfile.State m_setpoint = new ExponentialProfile.State(0, 0);

  // Standard classes for controlling our elevator
  private final PIDController m_pidController =
      new PIDController(Constants.kElevatorKp, Constants.kElevatorKi, Constants.kElevatorKd);

  ElevatorFeedforward m_feedforward =
      new ElevatorFeedforward(
          Constants.kElevatorkS,
          Constants.kElevatorkG,
          Constants.kElevatorkV,
          Constants.kElevatorkA);
  private final Encoder m_encoder =
      new Encoder(Constants.kEncoderAChannel, Constants.kEncoderBChannel);
  private final PWMSparkMax m_motor = new PWMSparkMax(Constants.kMotorPort);

  // Simulation classes help us simulate what's going on, including gravity.
  private final ElevatorSim m_elevatorSim =
      new ElevatorSim(
          m_elevatorGearbox,
          Constants.kElevatorGearing,
          Constants.kCarriageMass,
          Constants.kElevatorDrumRadius,
          Constants.kMinElevatorHeight,
          Constants.kMaxElevatorHeight,
          true,
          0,
          0.005,
          0.0);
  private final EncoderSim m_encoderSim = new EncoderSim(m_encoder);
  private final PWMMotorControllerSim m_motorSim = new PWMMotorControllerSim(m_motor);

  // Create a Mechanism2d visualization of the elevator
  private final Mechanism2d m_mech2d =
      new Mechanism2d(Units.inchesToMeters(10), Units.inchesToMeters(51));
  private final MechanismRoot2d m_mech2dRoot =
      m_mech2d.getRoot("Elevator Root", Units.inchesToMeters(5), Units.inchesToMeters(0.5));
  private final MechanismLigament2d m_elevatorMech2d =
      m_mech2dRoot.append(new MechanismLigament2d("Elevator", m_elevatorSim.getPosition(), 90));

  /** Subsystem constructor. */
  public Elevator() {
    m_encoder.setDistancePerPulse(Constants.kElevatorEncoderDistPerPulse);

    // Publish Mechanism2d to SmartDashboard
    // To view the Elevator visualization, select Network Tables -> SmartDashboard -> Elevator Sim
    SmartDashboard.putData("Elevator Sim", m_mech2d);
  }

  /** Advance the simulation. */
  public void simulationPeriodic() {
    // In this method, we update our simulation of what our elevator is doing
    // First, we set our "inputs" (voltages)
    m_elevatorSim.setInput(m_motorSim.getSpeed() * RobotController.getBatteryVoltage());

    // Next, we update it. The standard loop time is 20ms.
    m_elevatorSim.update(0.020);

    // Finally, we set our simulated encoder's readings and simulated battery voltage
    m_encoderSim.setDistance(m_elevatorSim.getPosition());
    // SimBattery estimates loaded battery voltages
    RoboRioSim.setVInVoltage(
        BatterySim.calculateDefaultBatteryLoadedVoltage(m_elevatorSim.getCurrentDraw()));
  }

  /**
   * Run control loop to reach and maintain goal.
   *
   * @param goal the position to maintain
   */
  public void reachGoal(double goal) {
    var goalState = new ExponentialProfile.State(goal, 0);

    var next = m_profile.calculate(0.020, m_setpoint, goalState);

    // With the setpoint value we run PID control like normal
    double pidOutput = m_pidController.calculate(m_encoder.getDistance(), m_setpoint.position);
    double feedforwardOutput = m_feedforward.calculate(m_setpoint.velocity, next.velocity);

    m_motor.setVoltage(pidOutput + feedforwardOutput);

    m_setpoint = next;
  }

  /** Stop the control loop and motor output. */
  public void stop() {
    m_motor.set(0.0);
  }

  /** Reset Exponential profile to begin from current position on enable. */
  public void reset() {
    m_setpoint = new ExponentialProfile.State(m_encoder.getDistance(), 0);
  }

  /** Update telemetry, including the mechanism visualization. */
  public void updateTelemetry() {
    // Update elevator visualization with position
    m_elevatorMech2d.setLength(m_encoder.getDistance());
  }

  @Override
  public void close() {
    m_encoder.close();
    m_motor.close();
    m_mech2d.close();
  }
}
