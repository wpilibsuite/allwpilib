// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.mechanism2d;

import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.smartdashboard.Mechanism2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismLigament2d;
import edu.wpi.first.wpilibj.smartdashboard.MechanismRoot2d;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

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

  private final MechanismLigament2d m_elevator;
  private final MechanismLigament2d m_wrist;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    m_elevatorEncoder.setDistancePerPulse(kMetersPerPulse);

    // the main mechanism object
    Mechanism2d mech = new Mechanism2d(3, 3);
    // the mechanism root node
    MechanismRoot2d root = mech.getRoot("climber", 2, 0);

    // MechanismLigament2d objects represent each "section"/"stage" of the mechanism, and are based
    // off the root node or another ligament object
    m_elevator = root.append(new MechanismLigament2d("elevator", kElevatorMinimumLength, 90));
    m_wrist =
        m_elevator.append(
            new MechanismLigament2d("wrist", 0.5, 90, 6, new Color8Bit(Color.kPurple)));

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
