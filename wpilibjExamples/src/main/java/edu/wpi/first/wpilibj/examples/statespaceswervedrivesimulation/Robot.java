/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespaceswervedrivesimulation;

import edu.wpi.first.wpilibj.*;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveOdometry;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.simulation.Field2d;
import edu.wpi.first.wpilibj.simulation.SimSwerveDrive;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.util.Units;

import java.util.List;

public class Robot extends TimedRobot {
  private static final Translation2d m_frontLeftLocation = new Translation2d(0.381, 0.381);
  private static final Translation2d m_frontRightLocation = new Translation2d(0.381, -0.381);
  private static final Translation2d m_backLeftLocation = new Translation2d(-0.381, 0.381);
  private static final Translation2d m_backRightLocation = new Translation2d(-0.381, -0.381);

  SimSwerveDrive sim = new SimSwerveDrive(
      50.0,
      LinearSystemId.createSingleJointedArmSystem(DCMotor.getNEO(1), .0005, 10.0),
      DCMotor.getNEO(1), 8.0, Units.inchesToMeters(2.0),
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation, m_backRightLocation
  );

  Field2d field = new Field2d();
  SwerveDriveKinematics kinematics = new SwerveDriveKinematics(
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation, m_backRightLocation);
  SwerveDriveOdometry odo = new SwerveDriveOdometry(kinematics, new Rotation2d());

  List<SwerveModule> swerves = List.of(
      new SwerveModule(0, new Encoder(0, 1),
          new PWMVictorSPX(0), new PWMVictorSPX(1)),
      new SwerveModule(1, new Encoder(2, 3),
          new PWMVictorSPX(2), new PWMVictorSPX(3)),
      new SwerveModule(2, new Encoder(4, 5),
          new PWMVictorSPX(4), new PWMVictorSPX(5)),
      new SwerveModule(3, new Encoder(6, 7),
          new PWMVictorSPX(6), new PWMVictorSPX(7))
  );

  final XboxController controller = new XboxController(0);

  @Override
  public void robotPeriodic() {
//    var refSpeed = ChassisSpeeds.fromFieldRelativeSpeeds(
//        -controller.getY(GenericHID.Hand.kRight), -controller.getX(GenericHID.Hand.kRight),
//        -controller.getX(GenericHID.Hand.kLeft), odo.getPoseMeters().getRotation());

//    refSpeed = new ChassisSpeeds(1, 1, 0);

    var refSpeed = new ChassisSpeeds(
        -applyDeadband(controller.getY(GenericHID.Hand.kRight), 0.1),
        -applyDeadband(controller.getX(GenericHID.Hand.kRight), 0.1),
        -applyDeadband(controller.getX(GenericHID.Hand.kLeft), 0.1));


    var refStates = kinematics.toSwerveModuleStates(refSpeed);
    SwerveDriveKinematics.normalizeWheelSpeeds(refStates, 1.0);

    for (int i = 0; i < 4; i++) {
      var swerve = swerves.get(i);
      swerve.distanceMotor.setVoltage(refStates[i].speedMetersPerSecond * 12);
//      swerve.angleMotor.setVoltage(1.0 * (refStates[i].angle.minus(sim.getEstimatedModuleAngles().get(i)).getRadians()));
      swerve.angleMotor.setVoltage(swerve.angleController.calculate(
          swerve.getState().angle.getRadians(), refStates[i].angle.getRadians()));

      SmartDashboard.putNumber("Angle" + i, sim.getEstimatedModuleAngles().get(i).getDegrees());
    }

    odo.update(sim.getEstimatedHeading(), swerves.stream().map(SwerveModule::getState).toArray(SwerveModuleState[]::new));
  }

  @Override
  public void simulationPeriodic() {
    sim.setModuleAzimuthVoltages(
        swerves.get(0).angleMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(1).angleMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(2).angleMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(3).angleMotor.get() * RobotController.getBatteryVoltage());
    sim.setModuleDriveVoltages(
        swerves.get(0).distanceMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(1).distanceMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(2).distanceMotor.get() * RobotController.getBatteryVoltage(),
        swerves.get(3).distanceMotor.get() * RobotController.getBatteryVoltage()
    );

    sim.update(0.020);
    var states = sim.getEstimatedModuleStates();

    for (int i = 0; i < states.length; i++) {
      swerves.get(i).angleEncoderSim.setPosition(states[i].angle);
      swerves.get(i).distanceEncoderSim.setRate(states[i].speedMetersPerSecond);
    }

    field.setRobotPose(odo.getPoseMeters());
  }

  protected double applyDeadband(double value, double deadband) {
    if (Math.abs(value) > deadband) {
      if (value > 0.0) {
        return (value - deadband) / (1.0 - deadband);
      } else {
        return (value + deadband) / (1.0 - deadband);
      }
    } else {
      return 0.0;
    }
  }
}
