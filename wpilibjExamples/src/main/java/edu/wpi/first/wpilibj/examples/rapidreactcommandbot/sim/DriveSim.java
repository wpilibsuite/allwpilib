package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim;

import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.DriveConstants;
import edu.wpi.first.wpilibj.simulation.DifferentialDrivetrainSim;
import edu.wpi.first.wpilibj.simulation.DifferentialDrivetrainSim.KitbotGearing;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;

public class DriveSim {
  private final PWMSim m_leftLeader = new PWMSim(DriveConstants.kLeftMotor1Port);
  private final PWMSim m_leftFollower = new PWMSim(DriveConstants.kLeftMotor2Port);
  private final PWMSim m_rightLeader = new PWMSim(DriveConstants.kRightMotor1Port);
  private final PWMSim m_rightFollower = new PWMSim(DriveConstants.kRightMotor2Port);

  private final EncoderSim m_leftEncoder =
      EncoderSim.createForChannel(DriveConstants.kLeftEncoderPorts[0]);
  private final EncoderSim m_rightEncoder =
      EncoderSim.createForChannel(DriveConstants.kRightEncoderPorts[0]);
  private final DifferentialDrivetrainSim m_drive =
      new DifferentialDrivetrainSim(
          LinearSystemId.identifyDrivetrainSystem(
              DriveConstants.kvVoltSecondsPerMeter, DriveConstants.kaVoltSecondsSquaredPerMeter,
              DriveConstants.kvVoltSecondsPerRadian, DriveConstants.kaVoltSecondsSquaredPerRadian),
          DCMotor.getNEO(2),
          DriveConstants.kDriveGearing,
          DriveConstants.kTrackwidthMeters,
          KitbotGearing.k10p71.value,
          null);

  public void simulationPeriodic() {
    double battery = RobotController.getBatteryVoltage();
    m_drive.setInputs(battery * m_leftLeader.getSpeed(), battery * m_rightLeader.getSpeed());

    m_drive.update(0.02);

    m_leftEncoder.setDistance(m_drive.getLeftPositionMeters());
    m_rightEncoder.setDistance(-m_drive.getRightPositionMeters());
  }
}
