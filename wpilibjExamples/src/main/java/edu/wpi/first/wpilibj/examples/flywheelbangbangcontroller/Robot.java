// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.flywheelbangbangcontroller;

import edu.wpi.first.math.controller.BangBangController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.simulation.FlywheelSim;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This is a sample program to demonstrate the use of a BangBangController with a flywheel to
 * control RPM.
 */
public class Robot extends TimedRobot {
  private static final int kMotorPort = 0;
  private static final int kEncoderAChannel = 0;
  private static final int kEncoderBChannel = 1;

  private final MotorController m_flywheelMotor = new PWMSparkMax(kMotorPort);
  private final Encoder m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);

  // Create Bang Bang controler
  private final BangBangController m_bangBangControler = new BangBangController();

  // Gains are for example purposes only - must be determined for your own robot!
  public static final double kFlywheelKs = 0.0001;
  public static final double kFlywheelKv = 0.000195;
  public static final double kFlywheelKa = 0.0003;
  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(kFlywheelKs, kFlywheelKv, kFlywheelKa);

  private final Joystick m_joystick = new Joystick(0); // Joystick to control setpoint
  private static final double kMaxSetpointValue = 6000; // Max value for joystick control

  // Simulation classes help us simulate our robot

  private static final double kFlywheelMomentOfInertia =
      0.5 * Units.lbsToKilograms(1.5) * Math.pow(Units.inchesToMeters(4), 2); // 1/2*M*R^2

  // Reduction between motors and encoder, as output over input. If the flywheel
  // spins slower than the motors, this number should be greater than one.
  private static final double kFlywheelGearing = 1.0;

  private final FlywheelSim m_flywheelSim =
      new FlywheelSim(DCMotor.getNEO(1), kFlywheelGearing, kFlywheelMomentOfInertia);
  private final EncoderSim m_encoderSim = new EncoderSim(m_encoder);

  @Override
  public void robotInit() {
    // Add bang bang controler to SmartDashboard and networktables.
    SmartDashboard.putData(m_bangBangControler);
  }

  @Override
  public void teleopPeriodic() {
    // Scale setpoint value between 0 and maxSetpointValue
    double setpoint = Math.max(0, m_joystick.getRawAxis(0) * kMaxSetpointValue);

    // Set setpoint and measurement of the bang bang controller
    double bangOutput = m_bangBangControler.calculate(m_encoder.getRate(), setpoint);

    // Controls a motor with the output of the BangBang controller and a feedforward
    // Shrinks the feedforward slightly to avoid overspeeding the shooter
    m_flywheelMotor.set(bangOutput + 0.9 * m_feedforward.calculate(setpoint));
  }

  /** Update our simulation. This should be run every robot loop in simulation. */
  @Override
  public void simulationPeriodic() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated velocities to our simulated encoder
    m_flywheelSim.setInputVoltage(m_flywheelMotor.get() * RobotController.getInputVoltage());
    m_flywheelSim.update(0.02);
    m_encoderSim.setRate(m_flywheelSim.getAngularVelocityRPM());
  }
}
