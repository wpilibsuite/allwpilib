/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.util.Units;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class SimSwerveDriveTest {
  private SimSwerveDrive m_sim = new SimSwerveDrive(50,
      new SimSwerveDrive.SimSwerveModule(DCMotor.getNEO(1), 8.0,
          Units.inchesToMeters(2.0), new Translation2d(1, 1),
          LinearSystemId.createSingleJointedArmSystem(DCMotor.getNEO(1), 0.01, 10.0)),
      new SimSwerveDrive.SimSwerveModule(DCMotor.getNEO(1), 8.0,
          Units.inchesToMeters(2.0), new Translation2d(1, -1),
          LinearSystemId.createSingleJointedArmSystem(DCMotor.getNEO(1), 0.01, 10.0)),
      new SimSwerveDrive.SimSwerveModule(DCMotor.getNEO(1), 8.0,
          Units.inchesToMeters(2.0), new Translation2d(-1, 1),
          LinearSystemId.createSingleJointedArmSystem(DCMotor.getNEO(1), 0.01, 10.0)),
      new SimSwerveDrive.SimSwerveModule(DCMotor.getNEO(1), 8.0,
          Units.inchesToMeters(2.0), new Translation2d(-1, -1),
          LinearSystemId.createSingleJointedArmSystem(DCMotor.getNEO(1), 0.01, 10.0)));

  @Test
  public void testDrivingForward() {
    var desiredSpeeds =
        m_sim.getKinematics().toSwerveModuleStates(new ChassisSpeeds(1, 2, 0.5));

    // seems like ~3.25 volts per meter per second
    for (int i = 0; i < 200; i++) {
      m_sim.setModuleDriveVoltages(7.5, 7.5, 7.5, 7.5);

      // simple P loop on heading error
      double[] moduleVoltages = new double[m_sim.getModules().length];
      for (int m = 0; m < m_sim.getModules().length; m++) {
        var module = m_sim.getModules()[m];
        var ref = desiredSpeeds[m].angle;
        var angle = module.getAzimuthAngle();
        var delta = ref.minus(angle);
        moduleVoltages[m] = delta.getRadians() * 10.0;
      }

      m_sim.setModuleAzimuthVoltages(moduleVoltages);

      m_sim.update(0.020);
    }
    assertEquals(1, m_sim.getEstimatedChassisSpeed().vxMetersPerSecond, 0.1);
    assertEquals(2, m_sim.getEstimatedChassisSpeed().vyMetersPerSecond, 0.1);
    assertEquals(0.5, m_sim.getEstimatedChassisSpeed().omegaRadiansPerSecond, 0.1);
  }
}
