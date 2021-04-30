// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.mechanism2d;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.PWMSparkMax;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.Mechanism2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismRoot2d;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * This sample program shows how to use Mechanism2d - a visual representation of arms, elevators,
 * and other mechanisms on dashboards; driven by a node-based API.
 *
 * <p>Ligaments are based on other ligaments or roots, and roots are contained in the base
 * Mechanism2d object.
 */
public class Robot extends TimedRobot {
  private static final double kMetersPerPulse = 0.01;
  private static final double kElevatorMinimumLength = 0.5;

  private final PWMSparkMax m_elevatorMotor = new PWMSparkMax(0);
  private final PWMSparkMax m_wristMotor = new PWMSparkMax(1);
  private final AnalogPotentiometer m_wristPot = new AnalogPotentiometer(1, 90);
  private final Encoder m_elevatorEncoder = new Encoder(0, 1);
  private final Joystick m_joystick = new Joystick(0);

  private MechanismLigament2d m_elevator;
  private MechanismLigament2d m_wrist;

  @Override
  public void robotInit() {
    m_elevatorEncoder.setDistancePerPulse(kMetersPerPulse);

    // the main mechanism object
    Mechanism2d mech = new Mechanism2d(200, 200);
    // the mechanism root node
    MechanismRoot2d root = mech.getRoot("climber", 80, 100);

    // MechanismLigament2d objects represent each "section"/"stage" of the mechanism, and are based
    // off the root node or another ligament object
    m_elevator = root.append(new MechanismLigament2d("elevator", 10, 90));
    m_wrist = m_elevator.append(new MechanismLigament2d("wrist", 6, 90));

    // post the mechanism to the dashboard
    SmartDashboard.putData("Mech2d", mech);
  }

  @Override
  public void robotPeriodic() {
    // update the dashboard mechanism's state
    m_elevator.setLength(kElevatorMinimumLength + m_elevatorEncoder.getDistance());
    m_wrist.setAngle(m_wristPot.get());
  }

  @Override
  public void teleopPeriodic() {
    m_elevatorMotor.set(m_joystick.getRawAxis(0));
    m_wristMotor.set(m_joystick.getRawAxis(1));
  }
}
