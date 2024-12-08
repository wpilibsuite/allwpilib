// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.flywheelbangbangcontroller;

import edu.wpi.first.math.controller.BangBangController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.TimedRobot;
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

  // Max setpoint for joystick control in RPM
  private static final double kMaxSetpointValue = 6000.0;

  // Joystick to control setpoint
  private final Joystick m_joystick = new Joystick(0);

  private final PWMSparkMax m_flywheelMotor = new PWMSparkMax(kMotorPort);
  private final Encoder m_encoder = new Encoder(kEncoderAChannel, kEncoderBChannel);

  private final BangBangController m_bangBangController = new BangBangController();

  // Gains are for example purposes only - must be determined for your own robot!
  public static final double kFlywheelKs = 0.0001; // V
  public static final double kFlywheelKv = 0.000195; // V/RPM
  public static final double kFlywheelKa = 0.0003; // V/(RPM/s)
  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(kFlywheelKs, kFlywheelKv, kFlywheelKa);

  // Simulation classes help us simulate our robot

  // Reduction between motors and encoder, as output over input. If the flywheel
  // spins slower than the motors, this number should be greater than one.
  private static final double kFlywheelGearing = 1.0;

  // 1/2 MR²
  private static final double kFlywheelMomentOfInertia =
      0.5 * Units.lbsToKilograms(1.5) * Math.pow(Units.inchesToMeters(4), 2);

  private final DCMotor m_gearbox = DCMotor.getNEO(1);

  private final LinearSystem<N1, N1, N1> m_plant =
      LinearSystemId.createFlywheelSystem(m_gearbox, kFlywheelGearing, kFlywheelMomentOfInertia);

  private final FlywheelSim m_flywheelSim = new FlywheelSim(m_plant, m_gearbox);
  private final EncoderSim m_encoderSim = new EncoderSim(m_encoder);

  public Robot() {
    // Add bang-bang controller to SmartDashboard and networktables.
    SmartDashboard.putData(m_bangBangController);
  }

  /** Controls flywheel to a set speed (RPM) controlled by a joystick. */
  @Override
  public void teleopPeriodic() {
    // Scale setpoint value between 0 and maxSetpointValue
    double setpoint =
        Math.max(
            0.0,
            m_joystick.getRawAxis(0)
                * Units.rotationsPerMinuteToRadiansPerSecond(kMaxSetpointValue));

    // Set setpoint and measurement of the bang-bang controller
    double bangOutput = m_bangBangController.calculate(m_encoder.getRate(), setpoint) * 12.0;

    // Controls a motor with the output of the BangBang controller and a
    // feedforward. The feedforward is reduced slightly to avoid overspeeding
    // the shooter.
    m_flywheelMotor.setVoltage(bangOutput + 0.9 * m_feedforward.calculate(setpoint));
  }

  /** Update our simulation. This should be run every robot loop in simulation. */
  @Override
  public void simulationPeriodic() {
    // To update our simulation, we set motor voltage inputs, update the
    // simulation, and write the simulated velocities to our simulated encoder
    m_flywheelSim.setInputVoltage(m_flywheelMotor.get() * RobotController.getInputVoltage());
    m_flywheelSim.update(0.02);
    m_encoderSim.setRate(m_flywheelSim.getAngularVelocityRadPerSec());
  }
}
