package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim;

import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.simulation.FlywheelSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;

public class ShooterSim {
  private final PWMSim m_shooterMotor = new PWMSim(ShooterConstants.kShooterMotorPort);
  private final PWMSim m_feederMotor = new PWMSim(ShooterConstants.kFeederMotorPort);
  private final EncoderSim m_shooterEncoder =
      EncoderSim.createForChannel(ShooterConstants.kEncoderPorts[0]);

  private final FlywheelSim m_flywheelSim =
      new FlywheelSim(
          LinearSystemId.identifyVelocitySystem(
              ShooterConstants.kVVoltSecondPerRadian,
              ShooterConstants.kAVoltSecondsSquaredPerRadian),
          DCMotor.getNEO(1),
          1.0);

  public void simulationPeriodic() {
    m_flywheelSim.setInputVoltage(m_shooterMotor.getSpeed() * RobotController.getBatteryVoltage());
    m_flywheelSim.update(0.02);

    m_shooterEncoder.setRate(m_flywheelSim.getAngularVelocityRPM() / 60.0);
  }
}
