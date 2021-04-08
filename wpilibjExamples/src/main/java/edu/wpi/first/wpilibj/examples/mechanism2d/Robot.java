package edu.wpi.first.wpilibj.examples.mechanism2d;

import edu.wpi.first.wpilibj.*;
import edu.wpi.first.wpilibj.smartdashboard.*;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

public class Robot extends TimedRobot {
  private static final double kMetersPerPulse = 0.1;

  private final PWMSparkMax m_elevatorMotor = new PWMSparkMax(0);
  private final PWMSparkMax m_wristMotor = new PWMSparkMax(1);
  private final AnalogPotentiometer m_wristPot = new AnalogPotentiometer(1, 180);
  private final Encoder m_elevatorEncoder = new Encoder(0, 1);
  private final Joystick m_joystick = new Joystick(0);

  private MechanismLigament2d m_elevator;
  private MechanismLigament2d m_wrist;

  @Override
  public void robotInit() {
    m_elevatorEncoder.setDistancePerPulse(kMetersPerPulse);

    Mechanism2d mech = new Mechanism2d(0, 0);
    MechanismRoot2d root = mech.getRoot("climber", 80, 100);
    m_elevator =
        root.append(
            new MechanismLigament2d("elevator", new Color8Bit(Color.kDarkOrange), 10, 0, 10));
    m_wrist =
        m_elevator.append(
            new MechanismLigament2d("wrist", new Color8Bit(Color.kForestGreen), 6, 90, 4));

    SmartDashboard.putData("Mech2d", mech);
  }

  @Override
  public void robotPeriodic() {
    m_elevator.setLength(m_elevatorEncoder.getDistance());
    m_wrist.setAngle(m_wristPot.get());
  }

  @Override
  public void teleopPeriodic() {
    m_elevatorMotor.set(m_joystick.getRawAxis(0));
    m_wristMotor.set(m_joystick.getRawAxis(1));
  }
}
