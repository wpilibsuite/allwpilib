/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.armsimulation;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.simulation.RoboRioSim;
import edu.wpi.first.wpilibj.simulation.BatterySim;
import edu.wpi.first.wpilibj.simulation.SingleJointedArmSim;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.VecBuilder;

/**
 * This is a sample program to demonstrate the use of elevator simulation with existing code.
 */
public class Robot extends TimedRobot {
  private static final int kMotorPort = 0;
  private static final int kEncoderAChannel = 0;
  private static final int kEncoderBChannel = 1;
  private static final int kJoystickPort = 0;

  // The P gain for the PID controller that drives this arm.
  private static final double kArmKp = 5.0;

  // distance per pulse = (angle per revolution) / (pulses per revolution)
  //  = (2 * PI rads) / (4096 pulses)
  private static final double kArmEncoderDistPerPulse = 2.0 * Math.PI / 4096;

  // The arm gearbox represents a gerbox containing two Vex 775pro motors.
  private final DCMotor m_armGearbox = DCMotor.getVex775Pro(2);

  // Standard classes for controlling our elevator
  private final PIDController m_controller = new PIDController(kArmKp, 0, 0);
  private final Encoder m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);
  private final PWMVictorSPX m_motor = new PWMVictorSPX(kMotorPort);
  private final Joystick m_joystick = new Joystick(kJoystickPort);

  // Simulation classes help us simulate what's going on, including gravity.
  private static final double m_armReduction = 100;
  private static final double m_armMass = 5.0; // Kilograms
  private static final double m_armLength = Units.inchesToMeters(30);
  // This arm sim represents an arm that can travel from -180 degrees (rotated straight backwards)
  // to 0 degrees (rotated straight forwards).
  private final SingleJointedArmSim m_armSim = new SingleJointedArmSim(m_armGearbox,
      m_armReduction,
      SingleJointedArmSim.estimateMOI(m_armLength, m_armMass),
      m_armLength,
      Units.degreesToRadians(-180),
      Units.degreesToRadians(0),
      m_armMass,
      true,
      VecBuilder.fill(Units.degreesToRadians(0.5)) // Add noise with a std-dev of 0.5 degrees
      );
  private final EncoderSim m_encoderSim = new EncoderSim(m_encoder);

  @Override
  public void robotInit() {
    m_encoder.setDistancePerPulse(kArmEncoderDistPerPulse);
  }

  @Override
  public void simulationPeriodic() {
    // In this method, we update our simulation of what our elevator is doing
    // First, we set our "inputs" (voltages)
    m_armSim.setInput(m_motor.get() * RobotController.getBatteryVoltage());

    // Next, we update it. The standard loop time is 20ms.
    m_armSim.update(0.020);

    // Finally, we set our simulated encoder's readings and simulated battery voltage
    m_encoderSim.setDistance(m_armSim.getAngleRads());
    // SimBattery estimates loaded battery voltages
    RoboRioSim.setVInVoltage(BatterySim.calculateDefaultBatteryLoadedVoltage(m_armSim.getCurrentDrawAmps()));
  }

  @Override
  public void teleopPeriodic() {
    if (m_joystick.getTrigger()) {
      // Here, we run PID control like normal, with a constant setpoint of 30in.
      var pidOutput = m_controller.calculate(m_encoder.getDistance(), Units.degreesToRadians(0));
      m_motor.setVoltage(pidOutput);
    } else {
      // Otherwise, we disable the motor.
      m_motor.set(0.0);
    }
  }

  @Override
  public void disabledInit() {
    // This just makes sure that our simulation code knows that the motor's off.
    m_motor.set(0.0);
  }
}
