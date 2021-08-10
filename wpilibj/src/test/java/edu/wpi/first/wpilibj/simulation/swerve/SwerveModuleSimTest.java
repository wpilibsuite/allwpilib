// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation.swerve;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import java.util.Arrays;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.drive.Vector2d;
import edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.utils.SimpleCSVWriter;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInfo;

public class SwerveModuleSimTest {

  final double SIM_TS = 0.020;
  final double WHEEL_RAD_M = Units.inchesToMeters(1.5);
  final double TEST_PLANT_MASS_RADPERSEC2_PER_N = 6.0 / 90.0;

  final Vector2d TEST_DIR_XHAT = new Vector2d(1, 0);
  final Vector2d TEST_DIR_YHAT = new Vector2d(0, 1);

  PWMVictorSPX wheelMotor;
  PWMVictorSPX azmthMotor;
  Encoder wheelEncoder;
  Encoder azmthEncoder;

  SwerveModuleSim sim = new SwerveModuleSim(
                              DCMotor.getCIM(1), 
                              DCMotor.getCIM(1), 
                              WHEEL_RAD_M, 
                              130, 
                              9.0, 
                              1.0, 
                              1.0, 
                              1.1,
                              0.8, 
                              16.0, 
                              0.001);

  private void initHardware(){
    wheelMotor = new PWMVictorSPX(2);
    azmthMotor = new PWMVictorSPX(3);
    wheelEncoder = new Encoder(4, 5);
    azmthEncoder = new Encoder(6, 7);
  }

  private void closeHardware(){
    wheelMotor.close();
    azmthMotor.close();
    wheelEncoder.close();
    azmthEncoder.close();
  }

  @Test
  @SuppressWarnings({ "LocalVariableName", "resource" })
  public void testBasicWheel(TestInfo testInfo) {

    SimpleCSVWriter log = new SimpleCSVWriter(testInfo.getDisplayName(),
        new ArrayList<String>(Arrays.asList("u_wheel", "y_wheelforce", "wheelvel", "u_azmth", "y_azmthPos")),
        new ArrayList<String>(Arrays.asList("V", "N", "RPM", "V", "rev")));

    var wheelVelRadPerSec = 0.0;
    var wheelPosRad = 0.0;

    initHardware();
    
    // Purposefully bad controller tuning to excite oscillation
    // which helps test coverage.
    var wheelController = new PIDController(3, 3, 0);
    wheelController.setIntegratorRange(-99999, 99999);

    var wheelEncoderSim = new EncoderSim(wheelEncoder);
    var azmthEncoderSim = new EncoderSim(azmthEncoder);

    for (int i = 0; i < 500; i++) {

      // ------ User Periodic -------

      wheelController.setSetpoint(100); // RPM

      double currentBatteryVoltage = RobotController.getBatteryVoltage();
      double nextWheelVoltage = wheelController.calculate(wheelEncoder.getRate());

      wheelMotor.set(nextWheelVoltage / currentBatteryVoltage);
      azmthMotor.set(0.0);

      // ------ Simulation Periodic -------

      var u_wheel = wheelMotor.get() * currentBatteryVoltage;
      var u_azmth = azmthMotor.get() * currentBatteryVoltage;
      sim.setInputVoltages(u_wheel, u_azmth);

      var modulePose = new Pose2d(TEST_DIR_XHAT.x * wheelPosRad * WHEEL_RAD_M,
          TEST_DIR_XHAT.y * wheelPosRad * WHEEL_RAD_M, new Rotation2d());
      sim.setModulePose(modulePose);

      sim.update(SIM_TS);

      // 1D Wheel plant model with non-rotating azimuth
      // Very simple test - F = ma
      var y_wheelforce = sim.getWheelMotiveForce();
      wheelVelRadPerSec += y_wheelforce.m_force.getVector2d().dot(TEST_DIR_XHAT) 
        * TEST_PLANT_MASS_RADPERSEC2_PER_N * SIM_TS;
      wheelEncoderSim.setRate(Units.radiansPerSecondToRotationsPerMinute(wheelVelRadPerSec));
      wheelPosRad += wheelVelRadPerSec * SIM_TS;

      // Azmth plant model is built into module simulation.
      var y_azmthPos = sim.getAzimuthEncoderPositionRev();
      azmthEncoderSim.setDistance(y_azmthPos);

      var logVals = new ArrayList<Double>();
      logVals.add(u_wheel);
      logVals.add(y_wheelforce.m_force.getVector2d().dot(TEST_DIR_XHAT));
      logVals.add(Units.radiansPerSecondToRotationsPerMinute(wheelVelRadPerSec));
      logVals.add(u_azmth);
      logVals.add(y_azmthPos);
      log.writeData(i * SIM_TS, logVals);
    }

    log.close();

    wheelController.close();

    var actSpd = wheelEncoder.getRate();
    var expSpd = wheelController.getSetpoint();

    closeHardware();

    assertEquals(expSpd, actSpd, 2);

  }

  @Test
  @SuppressWarnings({ "LocalVariableName", "resource" })
  public void testBasicAzmth(TestInfo testInfo) {

    SimpleCSVWriter log = new SimpleCSVWriter(testInfo.getDisplayName(),
        new ArrayList<String>(Arrays.asList("u_wheel", "y_wheelforce", "wheelvel", "u_azmth", "y_azmthPos")),
        new ArrayList<String>(Arrays.asList("V", "N", "RPM", "V", "rev")));

    var wheelVelRadPerSec = 0.0;
    var wheelPosRad = 0.0;

    initHardware();

    // Purposefully bad controller tuning to excite oscillation
    // which helps test coverage.
    var azmthController = new PIDController(100, 0, 0);
    azmthController.setIntegratorRange(-99999, 99999);

    var wheelEncoderSim = new EncoderSim(wheelEncoder);
    var azmthEncoderSim = new EncoderSim(azmthEncoder);

    for (int i = 0; i < 500; i++) {

      // ------ User Periodic -------

      azmthController.setSetpoint(0.5); // rev

      double currentBatteryVoltage = RobotController.getBatteryVoltage();
      double nextAzmthVoltage = azmthController.calculate(azmthEncoder.getDistance());

      wheelMotor.set(0.0);
      azmthMotor.set(nextAzmthVoltage / currentBatteryVoltage);

      // ------ Simulation Periodic -------

      var u_wheel = wheelMotor.get() * currentBatteryVoltage;
      var u_azmth = azmthMotor.get() * currentBatteryVoltage;
      sim.setInputVoltages(u_wheel, u_azmth);

      var modulePose = new Pose2d(TEST_DIR_XHAT.x * wheelPosRad * WHEEL_RAD_M,
          TEST_DIR_XHAT.y * wheelPosRad * WHEEL_RAD_M, new Rotation2d());
      sim.setModulePose(modulePose);

      sim.update(SIM_TS);

      // 1D Wheel plant model with non-rotating azimuth
      // Very simple test - F = ma
      var y_wheelforce = sim.getWheelMotiveForce();
      wheelVelRadPerSec += y_wheelforce.m_force.getVector2d().dot(TEST_DIR_XHAT) * TEST_PLANT_MASS_RADPERSEC2_PER_N
          * SIM_TS;
      wheelEncoderSim.setRate(Units.radiansPerSecondToRotationsPerMinute(wheelVelRadPerSec));
      wheelPosRad += wheelVelRadPerSec * SIM_TS;

      // Azmth plant model is built into module simulation.
      var y_azmthPos = sim.getAzimuthEncoderPositionRev();
      azmthEncoderSim.setDistance(y_azmthPos);

      var logVals = new ArrayList<Double>();
      logVals.add(u_wheel);
      logVals.add(y_wheelforce.m_force.getVector2d().dot(TEST_DIR_XHAT));
      logVals.add(Units.radiansPerSecondToRotationsPerMinute(wheelVelRadPerSec));
      logVals.add(u_azmth);
      logVals.add(y_azmthPos);
      log.writeData(i * SIM_TS, logVals);
    }

    log.close();

    azmthController.close();
    
    var actPos = azmthEncoder.getDistance();
    var expPos = azmthController.getSetpoint();

    closeHardware();

    assertEquals(expPos, actPos, 0.02);


  }

}
